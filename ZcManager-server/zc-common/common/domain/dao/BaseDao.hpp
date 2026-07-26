#pragma once

#include "framework/context/ContextHolder.h"
#include "common/exception/ServiceError.hpp"
#include "SqlBuilder.h"
#include "Function.h"
#include "common/base/Types.hpp"
#include <source_location>


#define DAO_INIT(CLASS_NAME)\
protected:\
	CLASS_NAME() {}\
public:\
	static std::shared_ptr<CLASS_NAME> createShared();\
	~CLASS_NAME() {}

#define DAO_IMPL(CLASS_NAME,REAL_NAME)\
std::shared_ptr<CLASS_NAME> CLASS_NAME::createShared()\
{\
	return std::make_shared<REAL_NAME>();\
}

template<typename EntityType,typename SubDao>
class BaseDao{
public:
	using Entity = EntityType;
public:
	/**
	 * 根据唯一ID查询实体.
	 */
	virtual Object<Entity> selectById(int32_t id, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 插入实体
	 */
	virtual bool insert(const Object<Entity>& e, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 更新实体
	 */
	virtual int32_t update(const Object<Entity>& e, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 删除实体
	 */
	virtual int32_t deleteByIds(const std::vector<int32_t>& ids, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

protected:
	inline static void handleException(const sql::SQLException& e, const std::string& query = "",const std::source_location& loc = std::source_location::current())
	{
		auto subDaoName = typeid(SubDao).name() + 6;
		std::cout<< std::format("[{}:{}]:",loc.file_name(),loc.line());
		std::cout << std::format("[{}] Error {}({}) : {}",subDaoName, e.getErrorCode(), e.getSQLState(), e.what()) << std::endl;
		if (!query.empty()) {
			std::cout << std::format("[{}] Query : {}", subDaoName, query) << std::endl;
		}
		throw ServiceError(std::format("数据库异常({}),请联系管理员~", e.getErrorCode()));
	}
};
