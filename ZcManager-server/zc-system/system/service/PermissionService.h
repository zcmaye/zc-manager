#pragma once

#include "MenuService.h"
#include "RoleService.h"

struct SysUser;

class PermissionService {
	MenuService m_menuService;
	RoleService m_roleService;
public:
	PermissionService();
	~PermissionService();

	/**
	 * 获取菜单数据权限.
	 */
	std::set<std::string> getMenuPermission(const Object<SysUser>& user);

	std::set<std::string> getRolePermission(const Object<SysUser>& user);
};

