#include "MenuService.h"
#include "../dao/MenuDao.h"

#include "common/constants/UserConstants.hpp"
#include "common/utils/StringUtils.h"
#include "common/domain/entity/SysUser.hpp"

MenuService::MenuService()
    :m_menuDao(MenuDao::createShared())
{}

MenuService::~MenuService()
{}

ObjectList<SysMenu> MenuService::getMenuTree(const Object<SysMenu>& menu)
{
    //查询菜单
    auto menus = m_menuDao->getMenuList(menu);
    //定义排除列表
    ObjectList<SysMenu> excludeList;
    //获取菜单的子菜单
    for (auto& m : menus) {
		m->children =  getChildPerms(menus, *m->menu_id);
        excludeList.insert(excludeList.end(), m->children.begin(), m->children.end());
    }
    //把所有是子菜单的删除掉
   auto delIt = std::remove_if(menus.begin(), menus.end(), [excludeList](const Object<SysMenu>& m) {
            return std::find(excludeList.begin(), excludeList.end(), m) != excludeList.end();
        });
    menus.erase(delIt, menus.end());
    return  menus;
}

ObjectList<SysMenu> MenuService::getMenus(const Object<SysMenu>& menu, int32_t userId)
{
    //如果是管理员
    if (SysUser::isAdmin(userId)) {
        return m_menuDao->getMenuList(menu);
    }
	return m_menuDao->getMenuListByUserId(menu, userId);
}

Object<SysMenu> MenuService::getMenuById(int32_t menuId)
{
   return  m_menuDao->selectById(menuId);
}

bool MenuService::deleteMenuById(int32_t menuId)
{
    return m_menuDao->deleteByIds({menuId});
}

ObjectList<SysMenu> MenuService::getMenuListByUserId(int32_t userId)
{
    ObjectList<SysMenu> menus;
    //如果是管理员
    if (SysUser::isAdmin(userId)) {
        menus = m_menuDao->getMenuList(std::make_shared<SysMenu>());
    }
    else {
		menus = m_menuDao->getMenuListByUserId(userId);
    }

    //定义排除列表
    ObjectList<SysMenu> excludeList;
    //获取菜单的子菜单
    for (auto& m : menus) {
		m->children =  getChildPerms(menus, *m->menu_id);
        excludeList.insert(excludeList.end(), m->children.begin(), m->children.end());
    }
    //把所有是子菜单的删除掉
   auto delIt = std::remove_if(menus.begin(), menus.end(), [excludeList](const Object<SysMenu>& m) {
            return std::find(excludeList.begin(), excludeList.end(), m) != excludeList.end();
        });
    menus.erase(delIt, menus.end());
    return  menus;
}

ObjectList<SysMenu> MenuService::getMenuTreeByUserId(int32_t userId)
{
	return getMenuListByUserId(userId);
}

std::set<int32_t> MenuService::getMenuListByRoleId( int32_t roleId)
{
    return m_menuDao->getMenuListByRoleId(roleId);
}

ObjectList<TreeSelect> MenuService::getMenuTreeSelect(const Object<SysMenu>& menu, int32_t userId)
{
    auto menus = getMenus(menu, userId);
    return buildMenuTreeSelect(menus);
}

bool MenuService::insertMenu(const Object<SysMenu>& menu)
{
    if (!checkMenuNameUnique(menu)) {
        throw std::runtime_error("菜单名已存在，请换一个~");
        return false;
    }
    return m_menuDao->insert(menu);
}

bool MenuService::updateMenu(const Object<SysMenu>& menu)
{
    if (!checkMenuNameUnique(menu)) {
        throw std::runtime_error("菜单名已存在，请换一个~");
        return false;
    }
    return m_menuDao->update(menu);
}

bool MenuService::deleteMenu(const Object<SysMenu>& menu)
{
    if (!menu->menu_id.has_value()) {
        throw std::runtime_error("删除失败，menu id错误~");
    }
    return m_menuDao->deleteByIds({ *menu->menu_id });
}

ObjectList<TreeSelect> MenuService::buildMenuTreeSelect(const ObjectList<SysMenu>& menus)
{
	auto menuTree = buildMenuTree(menus);

    ObjectList<TreeSelect> treeSelects;
	for (auto& m : menuTree) {
        treeSelects.push_back(std::make_shared<TreeSelect>(m));
    }
    return treeSelects;
}

ObjectList<SysMenu> MenuService::buildMenuTree(const ObjectList<SysMenu>& menus)
{
    ObjectList<SysMenu> returnList = menus;
    //定义排除列表
    ObjectList<SysMenu> excludeList;
    //获取菜单的子菜单
    for (auto& m : returnList) {
		m->children =  getChildPerms(returnList, *m->menu_id);
        excludeList.insert(excludeList.end(), m->children.begin(), m->children.end());
    }
    //把所有是子菜单的删除掉
    auto delIt = std::remove_if(returnList.begin(), returnList.end(), [excludeList](const Object<SysMenu>& m) {
            return std::find(excludeList.begin(), excludeList.end(), m) != excludeList.end();
        });
    returnList.erase(delIt, returnList.end());
    return  returnList;
}

std::set<std::string> MenuService::selectMenuPermsByRoleId(int32_t roleId)
{
    return m_menuDao->selectMenuPermsByRoleId(roleId);
}

std::set<std::string> MenuService::selectMenuPermsByUserId(int32_t userId)
{
    return m_menuDao->selectMenuPermsByUserId(userId);
}

ObjectList<SysMenu> MenuService::getChildPerms(const ObjectList<SysMenu>& menus, int32_t parentId)
{
    ObjectList<SysMenu> returnList;
    for (auto& m : menus) {
        //如果m是parenId的孩子
        if (m->parent_id == parentId) {
            recursionFn(menus, m);
            returnList.push_back(m);
        }
    }
    return returnList;
}

void MenuService::recursionFn(const ObjectList<SysMenu>& menus, const Object<SysMenu>& parent)
{
    //拿到parent的Child列表
    auto childList = getChildList(menus, parent);
    for (auto& child : childList) {
        if (hasChild(menus, child)) {
            recursionFn(menus, child);
        }
    }
    parent->children = childList;
}

ObjectList<SysMenu> MenuService::getChildList(const ObjectList<SysMenu>& menus, const Object<SysMenu>& parent)
{
    ObjectList<SysMenu> returnList;
    for (auto& m : menus) {
        if (m->parent_id == parent->menu_id) {
            returnList.push_back(m);
        }
    }
    return returnList;
}

bool MenuService::hasChild(const ObjectList<SysMenu>& menus, const Object<SysMenu>& parent)
{
    for (auto& m : menus) {
        if (m->parent_id == parent->menu_id) {
            return true;
        }
    }
    return false;
}

ObjectList<RouterVo> MenuService::buildMenus(const ObjectList<SysMenu>& menus)
{
    ObjectList<RouterVo> routers;
    for (auto& menu : menus) {
        auto router = std::make_shared<RouterVo>();
        router->hidden = menu->is_visible != 1;
        router->name = getRouterName(menu);
        router->path = getRouterPath(menu);
        router->component = getComponent(menu);
        router->meta = std::make_shared<MetaVo>(*menu->menu_name,*menu->icon,*menu->path);

        //如果有孩子
        if (!menu->children.empty() && menu->menu_type == UserConstants::TYPE_DIR) {
            router->redirect = UserConstants::NO_REDIRECT;
            router->children = buildMenus(menu->children);
        }
        //内链顶层菜单(类型为菜单)
        else if (isMenuFrame(menu)) {
			auto child = std::make_shared<RouterVo> ();
            child->path = *menu->path;
            child->component = *menu->component;
			child->name = StringUtils::capitalize(*menu->path);
			child->meta = std::make_shared<MetaVo>(*menu->menu_name, *menu->icon, *menu->path);
            router->children.push_back(child);
        }
        //内链顶层(菜单或目录)
        else if (menu->parent_id == 0&& isInnerLink(menu)) {
			router->meta = std::make_shared<MetaVo>(*menu->menu_name, *menu->icon);
            router->path = "/";
			auto child = std::make_shared<RouterVo> ();
            auto routerPath = innerLinkReplaceEach(*menu->path);
            child->path = routerPath;
            child->component = UserConstants::INNER_LINK;
			child->name = StringUtils::capitalize(*menu->path);
			child->meta = std::make_shared<MetaVo>(*menu->menu_name, *menu->icon, *menu->path);
            router->children.push_back(child);
        }
        routers.push_back(router);
    }
    return routers;
}

std::string MenuService::innerLinkReplaceEach(const std::string& path)
{
	static const char* before[] = { "http://","https://",".",":" };
	static const char* after[] = { "","","/","/"};

    auto s = path;
	for (int i = 0; i < sizeof(before) / sizeof(before[0]); i++) {
        StringUtils::replaceEach(s, before[i], after[i]);
    }
    return s;
}

bool MenuService::isParentView(const Object<SysMenu>& menu)
{
    return menu->parent_id != 0 && menu->menu_type == UserConstants::TYPE_DIR;
}

bool MenuService::isInnerLink(const Object<SysMenu>& menu)
{
    return !menu->is_frame && StringUtils::isHttp(*menu->path);
}

bool MenuService::isMenuFrame(const Object<SysMenu>& menu)
{
    return !menu->is_frame && menu->parent_id == 0 && menu->menu_type == UserConstants::TYPE_MENU;
}

std::string MenuService::getRouterName(const Object<SysMenu>& menu)
{
    auto routeName =  StringUtils::capitalize(*menu->path);

    //非外连接并且是一级目录(类型为目录)
    if (!menu->is_frame && menu->parent_id == 0 && menu->menu_type == UserConstants::TYPE_DIR) {
        routeName.clear();
    }

    return routeName;
}

std::string MenuService::getRouterPath(const Object<SysMenu>& menu)
{
    auto routerPath =  menu->path;
    //内链打开
    if (menu->parent_id != 0 && isInnerLink(menu)) {
        routerPath = innerLinkReplaceEach(*routerPath);
    }
    //非外链并且是一级目录(类型为目录)
    if (!menu->is_frame && menu->parent_id == 0 && menu->menu_type == UserConstants::TYPE_DIR) {
        routerPath = "/" + *menu->path;
    }
    //非外链并且是一级目录(类型为菜单)
    else {
        routerPath = "/";
    }
    return *routerPath;
}

std::string MenuService::getComponent(const Object<SysMenu>& menu)
{
    std::string component = UserConstants::LAYOUT;
    //如果组件为空，直接返回Layout
    if (!menu->component.has_value()) {
        return component;
    }
    if (!isMenuFrame(menu)) {
        component = *menu->component;
    }
    else if(menu->parent_id !=0 && isInnerLink(menu)) {
        component = UserConstants::INNER_LINK;
    }
    else if(isParentView(menu)){
        component = UserConstants::PARENT_VIEW;
    }
    return component;
}

bool MenuService::checkMenuNameUnique(const Object<SysMenu>& menu) const
{
    auto menuId = menu->menu_id.value_or(-1);

    auto info = m_menuDao->selectByName(*menu->menu_name, menu->parent_id.value_or(0));
    if (info && info->menu_id != menuId) {
        return false;
    }
    return true;
}

bool MenuService::hasChildByMenuId(int32_t menuId)
{
    return m_menuDao->hasChildByMenuId(menuId);
}

bool MenuService::checkMenuExistRole(int32_t menuId)
{
    return m_menuDao->checkMenuExistRole(menuId);
}

