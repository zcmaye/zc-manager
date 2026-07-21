#include "ContextHolder.h"
#include "sw/redis++/redis++.h"
#include "../config/AppConfig.h"

ContextHolder* ContextHolder::instance()
{
	static std::shared_ptr<ContextHolder> ins;
	if (!ins) {
		ins.reset(new ContextHolder);
	}
	return ins.get();
}

ContextHolder::ContextHolder()
{
	auto config = AppConfig::instance();

	//====================初始化redis
	//创建选项
	sw::redis::ConnectionOptions con_opt;
	con_opt.host = config->get<std::string>("host", "redis", "127.0.0.1");
	con_opt.port = config->get<int>("port", "redis", 6379);
	con_opt.password = config->get<std::string>("password", "redis", "123456");
	con_opt.db = config->get<int>("db", "redis", 0);

	//创建连接池选项
	sw::redis::ConnectionPoolOptions pool_opt;
	pool_opt.size = config->get<int>("max-size", "redis", 5);
	pool_opt.connection_idle_time = std::chrono::seconds(config->get<int>("idle-timeout", "redis", 300));
	pool_opt.connection_lifetime = std::chrono::seconds(config->get<int>("life-timeout", "redis", 600));
	pool_opt.wait_timeout = std::chrono::seconds(config->get<int>("wait-timeout", "redis", 10));

	redis = std::make_shared<sw::redis::Redis>(con_opt, pool_opt);

	//=======================初始化mysql
	mysql_pool = std::make_shared<zc::mysql::ConnectionPool>(
		//"127.0.0.1:3306", "root", "123456", "hdy_manager_system", 5
		config->get<std::string>("host", "mysql") + ":" + config->get<std::string>("port", "mysql"),
		config->get<std::string>("user", "mysql"),
		config->get<std::string>("password", "mysql"),
		config->get<std::string>("db", "mysql"),
		config->get<int>("max-size", "mysql")
	);
}
