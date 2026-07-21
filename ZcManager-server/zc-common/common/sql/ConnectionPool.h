#pragma once

#ifdef _WIN32
#include "mysql/jdbc.h"
#else
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#endif // _WIN32


#include <thread>
#include <mutex>
#include <condition_variable>

#include <memory>
#include <queue>
#include <chrono>
#include <functional>


//安全的从结果集中获取结果
#define SafeGetValue(result,getFun,var,name)\
	if(result->findColumn(name))\
		var = result->getFun(name)

#define SafeGetInt(result,var,name)\
	if(result->findColumn(name))\
		var = result->getInt(name)

#define SafeGetReal(result,var,name)\
	if(result->findColumn(name))\
		var = result->getDouble(name)

#define SafeGetBool(result,var,name)\
	if(result->findColumn(name))\
		var = result->getBoolean(name)

#define SafeGetString(result,var,name)\
	if(result->findColumn(name))\
		var = result->getString(name).asStdString()


namespace zc {
	namespace mysql {

		class ConnectionPool;

		/** 数据库连接  */
		class PooledConnection {
		public:
			PooledConnection();
			PooledConnection(std::unique_ptr<sql::Connection> conn, std::function<void(std::unique_ptr<sql::Connection>)> return_fn);

			~PooledConnection();

			// 禁止拷贝
			PooledConnection(const PooledConnection&) = delete;
			PooledConnection& operator=(const PooledConnection&) = delete;

			//允许移动
			PooledConnection(PooledConnection&& other);
			PooledConnection& operator=(PooledConnection&& other);

			inline void swap(PooledConnection& other) {
				std::swap(_conn, other._conn);
				std::swap(_return_fn, other._return_fn);
				std::swap(_returned, other._returned);
			}

			inline sql::Connection* operator->()const {
				return _conn.get();
			}

			inline sql::Connection* get()const {
				return _conn.get();
			}

			inline bool isValid() const {
				return !_returned && _conn != nullptr;
			}

			inline void returnToPool() {
				if (!_returned && _return_fn) {
					_return_fn(std::move(_conn));
					_returned = true;
				}
			}

			// 显式检查连接有效性
			bool validate();

			// 获取新连接（当当前连接失效时）
			bool reNewConnection(ConnectionPool& pool);

		private:
			std::unique_ptr<sql::Connection> _conn;
			std::function<void(std::unique_ptr<sql::Connection>)> _return_fn;
			bool _returned;

			inline void checkValid() const {
				if (_returned) {
					throw std::runtime_error("Connection already returned to pool");
				}
				if (!_conn) {
					throw std::runtime_error("Connection is null");
				}
			}
		};

		/** 数据库连接池 */
		class ConnectionPool
		{
		private:
			std::unique_ptr<sql::Connection> CreateConnection();
		public:
			//static std::shared_ptr<ConnectionPool> instance() {
			//	static std::shared_ptr<ConnectionPool> ins = std::make_shared<ConnectionPool>(
			//		"127.0.0.1:3306", "root", "123456", "hdy_manager_system", 5
			//	);
			//	return ins;
			//}

			ConnectionPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize, std::chrono::seconds validation_timeout = std::chrono::seconds(5));

			~ConnectionPool();

			PooledConnection getConnection();

			void returnConnection(std::unique_ptr<sql::Connection> conn);

			void close();

		private:
			void checkConnection();
			void checkConnectionPro();
			bool validateConnection(sql::Connection* conn);
			void safeCloseConnection(sql::Connection* conn);
		private:
			std::string url_;
			std::string user_;
			std::string pass_;
			std::string schema_;
			int poolSize_;
			std::chrono::seconds validation_timeout_;

			sql::mysql::MySQL_Driver* _driver;
			std::queue<std::unique_ptr<sql::Connection>> pool_;
			std::mutex mutex_;
			std::condition_variable cond_;
			std::atomic<bool> b_stop_;
			std::thread _check_thread;
			std::atomic<int> _fail_count;	//连接失败数量
		};

		/** 事务 */
		class Transaction {
		public:
			Transaction(PooledConnection&& conn)
				: _con(std::move(conn))
				, _commited(false) {
				_con->setAutoCommit(false);
			}

			Transaction(const std::shared_ptr<ConnectionPool>& pool)
				: _con(pool->getConnection())
				, _commited(false)
			{
				_con->setAutoCommit(false);
			}

			~Transaction() {
				rollback();
			}

			void commit() {
				if (_commited)
					return;
				_con->commit();
				_commited = true;
				_con->setAutoCommit(true);
			}

			void rollback() {
				if (_commited) {
					return;
				}
				try {
					_con->rollback();
					_commited = true;
					_con->setAutoCommit(true);
				}
				catch (const std::exception& e) {
					std::printf("%s", e.what());
				}
			}

			const PooledConnection& getConnection() {
				return _con;
			}
		private:
			PooledConnection _con;
			bool _commited;
		};
	}
}
