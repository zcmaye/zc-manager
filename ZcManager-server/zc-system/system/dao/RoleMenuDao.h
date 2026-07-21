#pragma once

#include "common/domain/dao/BaseDao.hpp"

#include "common/domain/entity/SysRole.hpp"
#include "system/domain/entity/SysRoleMenu.hpp"


class RoleMenuDao  : public BaseDao<SysRoleMenu,RoleMenuDao>
{
	DAO_INIT(RoleMenuDao)
public:
	virtual bool batchInsertRoleMenu(int32_t roleId,const std::vector<int>& menuIds,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
};

