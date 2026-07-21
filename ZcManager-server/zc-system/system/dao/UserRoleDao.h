#pragma once

#include "common/domain/dao/BaseDao.hpp"

#include "common/domain/entity/SysRole.hpp"
#include "system/domain/entity/SysRoleMenu.hpp"
#include "system/domain/entity/SysUserRole.hpp"


class UserRoleDao  : public BaseDao<SysUserRole,UserRoleDao>
{
	DAO_INIT(UserRoleDao);
public:
	/**
	 * 统计指定角色分配给了几个用户.
	 */
	virtual int countUserRoleByRoleId(int32_t roleId,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 取消授权用户角色.
	 */
	virtual bool deletedUserRoleInfo(const Object<SysUserRole>& userRole,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 批量取消授权用户角色.
	 */
	virtual bool deletedUserRoleInfos(int32_t roleId, const std::vector<int32_t> userIds,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 批量选择授权用户角色.
	 */
	virtual bool batchUserRole(int32_t roleId, const std::vector<int32_t> userIds,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

public:
	/**
	 * 批量插入用户角色.
	 */
	virtual bool batchUserRole(const ObjectList<SysUserRole>& list, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 删除用户的所有关联角色.
	 */
	virtual bool deleteUserRoleByUserId(int32_t userId,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
};

