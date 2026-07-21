#pragma once

#include "common/domain/dao/BaseDao.hpp"
#include "common/domain/entity/SysRole.hpp"

class RoleDao : public BaseDao < SysRole,RoleDao>
{
	DAO_INIT(RoleDao);
public:
	virtual ObjectList<SysRole> selectRoleList(const Object<SysRole>& role = std::make_shared<SysRole>(), int32_t page = 1, int32_t pageSize = 10, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
	virtual bool updateRoleStatus(const Object<SysRole>& role, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
public:
	/**
	 * 根据用户ID查询此用户的所有角色.
	 */
	virtual ObjectList<SysRole> selectRoleListByUserId(int32_t userId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
public:
	virtual Object<SysRole> selectRoleByName(const std::string& roleName, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION)const = 0;
	virtual Object<SysRole> selectRoleByKey(const std::string& roleKey, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION)const = 0;
};


