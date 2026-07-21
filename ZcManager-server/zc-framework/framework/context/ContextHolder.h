#pragma once

#include <memory>
#include "common/sql/ConnectionPool.h"

namespace sw {
	namespace redis { 
		class Redis; 
	}
}

#define DEFAULT_CONNECTION_POOL() ContextHolder::instance()->mysql_pool
#define DEFAULT_CONNECTION DEFAULT_CONNECTION_POOL()->getConnection()

class ContextHolder {
public:
	std::shared_ptr<sw::redis::Redis> redis;
	std::shared_ptr<zc::mysql::ConnectionPool> mysql_pool;
public:
	static ContextHolder* instance();

	ContextHolder();
};
