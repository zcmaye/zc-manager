#pragma once

#include "common/domain/dao/BaseDao.hpp"
#include "common/domain/entity/SysMenu.hpp"
#include <set>

class MenuDao  : public BaseDao<SysMenu,MenuDao>
{
	DAO_INIT(MenuDao);
public:
	virtual ObjectList<SysMenu> getMenuList(const Object<SysMenu>& menu = std::make_shared<SysMenu>(), const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
public:
	/**
	 * 获取指定用户的菜单列表.
	 */
	virtual ObjectList<SysMenu> getMenuListByUserId(int32_t userId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
	virtual ObjectList<SysMenu> getMenuListByUserId(const Object<SysMenu>& menu,int32_t userId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 获取指定角色的菜单列表.
	 */
	virtual std::set<int32_t> getMenuListByRoleId(int32_t roleId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 查询角色的权限.
	 */
	virtual std::set<std::string> selectMenuPermsByRoleId(int32_t roleId,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 查询用户的权限.
	 */
	virtual std::set<std::string> selectMenuPermsByUserId(int32_t userId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
public:
	virtual Object<SysMenu> selectByName(const std::string& menuName, int32_t parentId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 判断菜单是否有子菜单.
	 */
	virtual bool hasChildByMenuId(int32_t menuId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 判断菜单是否以及分配了.
	 */
	virtual bool checkMenuExistRole(int32_t menuId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

};

