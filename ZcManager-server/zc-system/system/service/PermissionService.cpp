#include "PermissionService.h"
#include "common/domain/entity/SysUser.hpp"
#include "common/domain/entity/SysRole.hpp"

PermissionService::PermissionService()
{}

PermissionService::~PermissionService()
{}

std::set<std::string> PermissionService::getMenuPermission(const Object<SysUser>&user) 
{
    std::set<std::string> perms;
    //管理员拥有所有权限
    if (user->isAdmin()) {
        perms.insert("*:*:*");
    }
    else {
		auto& roles = user->roles;
        if (!roles.empty()) {
            //根据角色查权限
            for (auto& role : roles) {
               auto rolePerms =  m_menuService.selectMenuPermsByRoleId(*role->role_id);
               for (auto& perm : rolePerms) {
                   perms.insert(perm);
               }
            }
        }
        else {
            //根据用户查权限
			perms = m_menuService.selectMenuPermsByUserId(*user->user_id);
        }
    }
	return perms;
}

std::set<std::string> PermissionService::getRolePermission(const Object<SysUser>& user)
{
    std::set<std::string> roles;
    if (user->isAdmin()) {
        roles.insert("admin");
    }
    else {
        roles = m_roleService.selectRolePermissionByUserId(*user->user_id);
    }
    return roles;
}

