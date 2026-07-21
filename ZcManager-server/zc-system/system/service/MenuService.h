#pragma once

#include "common/domain/entity/SysMenu.hpp"
#include "common/domain/vo/RouterVo.hpp"
#include "common/domain/vo/TreeSelect.hpp"

class MenuDao;

class MenuService {
public:
	MenuService();
	~MenuService();

	/**
	 * 获取菜单管理菜单树
	 */
	ObjectList<SysMenu> getMenuTree(const Object<SysMenu>& menu = {});

	ObjectList<SysMenu> getMenus(const Object<SysMenu>& menu, int32_t userId);

	/**
	 * 获取菜单选择树
	 */
	ObjectList<TreeSelect> getMenuTreeSelect(const Object<SysMenu>& menu, int32_t userId);


	bool insertMenu(const Object<SysMenu>& menu);
	bool updateMenu(const Object<SysMenu>& menu);
	bool deleteMenu(const Object<SysMenu>& menu);

public:
	/**
	 * 获取指定用户菜单列表.
	 */
	ObjectList<SysMenu> getMenuListByUserId(int32_t userId);
	ObjectList<SysMenu> getMenuTreeByUserId(int32_t userId);
	Object<SysMenu> getMenuById(int32_t menuId);
	bool deleteMenuById(int32_t menuId);


	/**
	 * 获取指定角色菜单列表.
	 */
	std::set<int32_t> getMenuListByRoleId(int32_t roleId);


	/**
	 * 构建菜单选择树.
	 */
	ObjectList<TreeSelect> buildMenuTreeSelect(const ObjectList<SysMenu>& menus);


	/**
	 * 构建菜单树.
	 */
	ObjectList<SysMenu> buildMenuTree(const ObjectList<SysMenu>& menus);
public:
	/**
	 * 查询角色的权限.
	 */
	std::set<std::string> selectMenuPermsByRoleId(int32_t roleId);

	/**
	 * 查询用户的权限.
	 */
	std::set<std::string> selectMenuPermsByUserId(int32_t userId);
private:
	/**
	 * 把menus中的所有parent_id为parentId的孩子都放到他的父节点中.
	 */
	ObjectList<SysMenu> getChildPerms(const ObjectList<SysMenu>& menus,int32_t parentId);

	/**
	 * 递归获取parent的孩子.
	 */
	void recursionFn(const ObjectList<SysMenu>& menus, const Object<SysMenu>& parent);

	/**
	 * 获取孩子列表.
	 * @param menus 在这个列表中查找孩子啊
	 * @param parent 查找这个的孩子
	 * @return 返回parent的所有孩子列表
	 */
	ObjectList<SysMenu>  getChildList(const ObjectList<SysMenu>& menus, const Object<SysMenu>& parent);

	/**
	 * 判断是否有孩子.
	 */
	bool hasChild(const ObjectList<SysMenu>& menus, const Object<SysMenu>& parent);
public:
	/**
	 * 根据menus构建路由列表.
	 */
	ObjectList<RouterVo> buildMenus(const ObjectList<SysMenu>& menus);
private:

	/**
	 * 替换内链
	 * @param path https://www.bilibili.com
	 * @return     www/bilibili/com
	 */
	std::string innerLinkReplaceEach(const std::string& path);

	/**
	 * 是否是parent view.
	 */
	bool isParentView(const Object<SysMenu>& menu);

	/**
	 * 是否是内链接.
	 */
	bool isInnerLink(const Object<SysMenu>& menu);

	/**
	 * 是否是菜单内连接.
	 */
	bool isMenuFrame(const Object<SysMenu>& menu);

	/**
	 * 获取路由名.
	 */
	std::string getRouterName(const Object<SysMenu>& menu);

	/**
	 * 获取路由路径.
	 */
	std::string getRouterPath(const Object<SysMenu>& menu);

	/**
	 * 获取路由组件.
	 */
	std::string getComponent(const Object<SysMenu>& menu);
public:
	bool checkMenuNameUnique(const Object<SysMenu>& menu)const;

	/**
	 * 判断菜单是否有子菜单.
	 */
	bool hasChildByMenuId(int32_t menuId);

	/**
	 * 判断菜单是否以及分配了.
	 */
	bool checkMenuExistRole(int32_t menuId);
private:
	std::shared_ptr<MenuDao > m_menuDao;
};

