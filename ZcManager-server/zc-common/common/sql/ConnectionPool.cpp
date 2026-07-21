#include "ConnectionPool.h"
#include "../base/Defer.hpp"

#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include <chrono>
#include <functional>

namespace zc {
	namespace mysql {
		PooledConnection::PooledConnection()
			: _returned(false)
		{
		}
		/** 数据库连接  */
		PooledConnection::PooledConnection(std::unique_ptr<sql::Connection> conn, std::function<void(std::unique_ptr<sql::Connection>)> return_fn)
			:_conn(std::move(conn))
			, _return_fn(std::move(return_fn))
			, _returned(false)
		{
		}

		PooledConnection::~PooledConnection() {
			if (!_returned && _return_fn) {
				_return_fn(std::move(_conn));
			}
		}

		//允许移动
		PooledConnection::PooledConnection(PooledConnection&& other)
			:_conn(std::move(other._conn))
			, _return_fn(std::move(other._return_fn))
			, _returned(other._returned)
		{
		}

		PooledConnection& PooledConnection::operator=(PooledConnection&& other) {
			_conn = std::move(other._conn);
			_return_fn = std::move(other._return_fn);
			_returned = other._returned;
			other._returned = false;
			return *this;
		}

		// 显式检查连接有效性
		bool PooledConnection::validate() {
			if (!isValid()) return false;

			try {
				std::unique_ptr<sql::Statement> stmt(_conn->createStatement());
				std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1"));
				return res->next() && res->getInt(1) == 1;
			}
			catch (const sql::SQLException& e) {
				std::printf("Connection validation failed: %s\n", e.what());
				return false;
			}
		}

		// 获取新连接（当当前连接失效时）
		bool PooledConnection::reNewConnection(ConnectionPool& pool) {
			if (_returned) return false;

			// 归还当前连接
			returnToPool();

			// 获取新连接
			try {
				*this = pool.getConnection();
				return isValid();
			}
			catch (const std::exception& e) {
				std::printf("Failed to renew connection: %s\n", e.what());
				return false;
			}
		}

		/** 数据库连接池 */
		std::unique_ptr<sql::Connection>  ConnectionPool::CreateConnection()
		{
			try {
				auto* conn = _driver->connect(url_, user_, pass_);
				conn->setSchema(schema_);

				// 设置连接超时
				conn->setClientOption("OPT_CONNECT_TIMEOUT", &validation_timeout_);
				conn->setClientOption("OPT_READ_TIMEOUT", &validation_timeout_);

				//std::unique_ptr<sql::Statement> stmt(conn->createStatement());
				//stmt->execute("SET NAMES GBK");

				//std::printf("Create Connection\n");
				return std::unique_ptr<sql::Connection>(conn);
			}
			catch (const std::exception& e) {
				std::printf("Failed to create connection:%s\n", e.what());
				return nullptr;
			}
		}
		ConnectionPool::ConnectionPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize, std::chrono::seconds validation_timeout)
			: url_(url), user_(user), pass_(pass), schema_(schema), poolSize_(poolSize), validation_timeout_(validation_timeout)
			, b_stop_(false)
			, _driver(sql::mysql::get_mysql_driver_instance())
		{
			try {
				for (int i = 0; i < poolSize_; ++i) {
					auto con = CreateConnection();
					if (con) {
						pool_.push(std::move(con));
					}
				}

				_check_thread = std::thread([this]() {
					std::this_thread::sleep_for(std::chrono::seconds(10));
					while (!b_stop_) {
						checkConnectionPro();
						std::this_thread::sleep_for(std::chrono::seconds(20));
					}
					});

				_check_thread.detach();
			}
			catch (sql::SQLException& e) {
				// 处理异常
				std::printf("mysql pool init failed, error is:%s\n", e.what());
			}
		}

		ConnectionPool::~ConnectionPool() {
			close();
		}

		void  ConnectionPool::checkConnection() {
			std::lock_guard<std::mutex> guard(mutex_);
			int poolsize = pool_.size();

			for (int i = 0; i < poolsize; i++) {
				auto con = std::move(pool_.front());
				pool_.pop();

				Defer defer([this, &con]() {
					pool_.push(std::move(con));
					});

				if (!validateConnection(con.get())) {
					// 重新创建连接并替换旧的连接
					con = CreateConnection();
				}
			}
		}

		bool ConnectionPool::validateConnection(sql::Connection* conn)
		{
			if (!conn || conn->isClosed()) {
				return false;
			}
			try {
				return conn->isValid();
			}
			catch (sql::SQLException& e) {
				std::printf("Connection validate failed: %s\n", e.what());
				// 重新创建连接并替换旧的连接
				return false;
			}
			return true;
		}

		void  ConnectionPool::checkConnectionPro() {
			//1,获取当前连接数
			size_t poolSize;
			{
				std::lock_guard<std::mutex> guard(mutex_);
				poolSize = pool_.size();
				//如果连接池里面没有连接，则直接则直接创建连接
				if (pool_.empty()) {
					_fail_count = poolSize_;
				}
			}
			//2,当前已经检查的连接数
			size_t checkedSize = 0;
			while (checkedSize < poolSize)
			{
				std::unique_ptr<sql::Connection> conn;
				//获取头部连接
				{
					std::lock_guard<std::mutex> guard(mutex_);
					if (pool_.empty()) {
						break;
					}
					conn = std::move(pool_.front());
					pool_.pop();
				}

				//做健康检查
				if (validateConnection(conn.get())) {
					std::lock_guard<std::mutex> guard(mutex_);
					pool_.push(std::move(conn));
					cond_.notify_one();
				}
				else {
					_fail_count++;
				}
				checkedSize++;
			}

			//3,重连检查失败的连接
			while (_fail_count > 0) {
				auto conn =  CreateConnection();
				if (conn) {
					std::lock_guard<std::mutex> guard(mutex_);
					pool_.push(std::move(conn));
					_fail_count--;
				}
				else {
					break;
				}
			}
		}


		PooledConnection ConnectionPool::getConnection() {
			std::unique_lock<std::mutex> lock(mutex_);
			cond_.wait(lock, [this] {
				if (b_stop_) {
					return true;
				}
				return !pool_.empty(); });
			if (b_stop_) {
				throw std::runtime_error("Connection pool is shutdown");
			}
			auto con = std::move(pool_.front());
			pool_.pop();
			return PooledConnection(std::move(con), [this](std::unique_ptr<sql::Connection> con_to_return) {
				//std::printf("Return Connection~\n");
				this->returnConnection(std::move(con_to_return));
				});
		}

		void  ConnectionPool::returnConnection(std::unique_ptr<sql::Connection> conn) {
			std::unique_lock<std::mutex> lock(mutex_);
			if (b_stop_) {
				return;
			}
			// 检查连接是否仍然有效
			if (!validateConnection(conn.get())) {
				std::printf("Returned connection is invalid, replacing...\n");

				safeCloseConnection(conn.get());

				// 创建新连接替代
				auto new_conn = CreateConnection();
				if (new_conn) {
					pool_.push(std::move(new_conn));
					cond_.notify_one();
				}
				return;
			}
			pool_.push(std::move(conn));
			cond_.notify_one();
		}

		void ConnectionPool::close() {
			if (b_stop_.exchange(true))
				return;

			std::lock_guard<std::mutex> lock(mutex_);
			while (!pool_.empty()) {
				auto conn = std::move(pool_.front());
				pool_.pop();
				safeCloseConnection(conn.get());
			}
			cond_.notify_all();
		}

		void ConnectionPool::safeCloseConnection(sql::Connection* conn) {
			if (!conn) return;
			try {
				if (!conn->isClosed()) {
					conn->close();
				}
			}
			catch (const sql::SQLException& e) {
				std::printf("Error closing connection: %s\n", e.what());
			}
		}
	}
}
