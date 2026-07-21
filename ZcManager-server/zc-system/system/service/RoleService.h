#pragma once

#include "common/base/Singleton.hpp"
#include "common/domain/entity/SysRole.hpp"
#include "common/domain/vo/PagingVo.hpp"
#include "framework/context/ContextHolder.h"

struct SysUserRole;
class RoleDao;
class RoleMenuDao;
class UserRoleDao;

class RoleService  {
public:
	RoleService();
	~RoleService();

	std::shared_ptr<PagingVo<SysRole>> selectRoleList(const std::shared_ptr<SysRole>& role, int32_t page = 1, int32_t pageSize = 10);
	ObjectList<SysRole> selectRoleAll(const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION);
	Object<SysRole> selectRoleById(int32_t roleId);
	std::set<std::string> selectRolePermissionByUserId(int32_t userId);
	bool  updateRole(const std::shared_ptr<SysRole>& role);
	bool  updateRoleStatus(const std::shared_ptr<SysRole>& role);
	bool  deleteRole(const std::shared_ptr<SysRole>& role);
	bool  deleteRoles(const std::vector<int>& roleIds);
	bool  insertRole(const std::shared_ptr<SysRole>& role);
public:
	/**
	 * 取消授权用户角色.
	 */
	bool deletedAuthUser(const std::shared_ptr<SysUserRole>& userRole);

	/**
	 * 批量取消授权用户角色.
	 */
	bool deletedAuthUsers(int32_t roleId, const std::vector<int32_t> userIds);

	/**
	 * 批量选择授权用户角色.
	 */
	bool insertAuthUsers(int32_t roleId, const std::vector<int32_t> userIds);
public:
	void checkRoleAllowed(int32_t  roleId)const;
	bool checkRoleNameUnique(const std::shared_ptr<SysRole>& role);
	bool checkRoleKeyUnique(const std::shared_ptr<SysRole>& role);
private:
	std::shared_ptr<RoleDao> m_roleDao;
	std::shared_ptr<RoleMenuDao> m_roleMenuDao;
	std::shared_ptr<UserRoleDao> m_userRoleDao;
};

