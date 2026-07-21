#include "../MenuController.h"

#include "system/service/MenuService.h"
#include "common/constants/UserConstants.hpp"
#include "common/utils/StringUtils.h"
#include "web/utils/QueryParamsMapping.hpp"

class MenuControllerImpl : public MenuController
{
	MenuService m_menuService;
public:
	void registerRouters(hv::HttpService* router) override
	{
		ENDPOINT(router, GET, "/menu/tree", tree);
		ENDPOINT(router, GET, "/menu/treeselect", treeselect);
		ENDPOINT(router, GET, "/menu/checkedKeys/{roleId}", checkedKeys);
		ENDPOINT(router, GET, "/menu/roleMenuTreeselect/{roleId}", roleMenuTreeselect);
		ENDPOINT(router, POST, "/menu/add", add);
		ENDPOINT(router, PUT, "/menu/edit", edit);
		ENDPOINT(router, Delete, "/menu/{menuId}", remove);

		//这个端点必须放到最后，否则会覆盖掉/menu/list
		ENDPOINT(router, GET, "/menu/{menuId}", getInfo);
	}
	int tree(const HttpContextPtr& ctx) override
	{
		auto menu = QueryParamsMapping::mapTo<SysMenu>(ctx->params());

		//调用服务
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_menuService.getMenuTree(menu)));
	}


	int getInfo(const HttpContextPtr& ctx) override
	{
		//检查参数
		int32_t menuId;
		if (auto v = ctx->param("menuId"); v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少菜单ID"));
		}
		else {
			menuId = std::stoi(v);
			if (menuId <= 0) {
				return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少菜单ID"));
			}
		}

		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_menuService.getMenuById(menuId)));
	}

	int add(const HttpContextPtr& ctx) override
	{
		auto menu = std::make_shared<SysMenu>();
		*menu = hv::Json::parse(ctx->body());

		//检查参数
		if (!menu->menu_name) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增菜单失败，缺少菜单名~"));
		}
		else if (!menu->parent_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增菜单失败，缺少父菜单"));
		}
		else if (!menu->is_frame) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增菜单失败，缺少外链标识"));
		}

		//检查唯一性
		if (!m_menuService.checkMenuNameUnique(menu)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增菜单'" + *menu->menu_name + "'失败，菜单名已存在~"));
		}
		else if (menu->is_frame == UserConstants::YES_FRAME && !StringUtils::isHttp(*menu->path)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增菜单'" + *menu->menu_name + "'失败，地址必须以http(s)开头~"));
		}

		//创建人
		menu->create_by = getUsername();

		//执行插入
		m_menuService.insertMenu(menu);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("菜单新增成功~"));
	}

	int edit(const HttpContextPtr& ctx) override
	{
		auto menu = std::make_shared<SysMenu>();
		*menu = hv::Json::parse(ctx->body());

		//检查参数
		if (!menu->menu_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改菜单失败，缺少菜单ID~"));
		}

		//检查唯一性
		if (!menu->parent_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改菜单失败，父菜单缺失~"));
		}
		else if (menu->menu_name && !m_menuService.checkMenuNameUnique(menu)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改菜单'" + *menu->menu_name + "'失败，菜单名称已存在~"));
		}
		else if (menu->menu_id == menu->parent_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改菜单'" + *menu->menu_name + "'失败，上级菜单不能是自己~"));
		}
		else if (menu->is_frame == UserConstants::YES_FRAME && !StringUtils::isHttp(*menu->path)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增菜单'" + *menu->menu_name + "'失败，地址必须以http(s)开头~"));
		}

		//创建人
		menu->update_by = getUsername();

		//执行插入
		m_menuService.updateMenu(menu);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("菜单更新成功~"));
	}

	int remove(const HttpContextPtr& ctx) override
	{
		//获取要删除的菜单ID
		int32_t menuId;
		if (auto v = ctx->param("menuId"); v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("删除失败，缺少菜单ID~"));
		}
		else {
			menuId = std::stoi(v);
		}

		if (m_menuService.hasChildByMenuId(menuId)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("存在下级菜单，不允许删除~"));
		}
		if (m_menuService.checkMenuExistRole(menuId)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("菜单已分配角色，不允许删除~"));
		}

		//删除
		m_menuService.deleteMenuById({menuId});
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("删除成功~"));
	}

public:
	int treeselect(const HttpContextPtr& ctx) override
	{
		auto menu = std::make_shared<SysMenu>();
		if (auto v = ctx->param("menu_name");!v.empty()) {
			menu->menu_name = v;
		}
		if (auto v = ctx->param("is_active");!v.empty()) {
			menu->is_active = std::stoi(v);
		}
		if (auto v = ctx->param("is_visible");!v.empty()) {
			menu->is_visible = std::stoi(v);
		}

		
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_menuService.getMenuTreeSelect(menu, getUserId())));
	}

	int checkedKeys(const HttpContextPtr& ctx) override
	{
		int32_t roleId;
		if (auto v = ctx->param("roleId");v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少角色ID~"));
		}
		else {
			roleId = std::stoi(v);
		}
		auto result = Result::success("删除成功~");
		result.put("checkedKeys", m_menuService.getMenuListByRoleId(roleId));
		return sendResult(ctx, HTTP_STATUS_OK,result);
	}

	int roleMenuTreeselect(const HttpContextPtr& ctx) override
	{
		int32_t roleId;
		if (auto v = ctx->param("roleId");v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少角色ID~"));
		}
		else {
			roleId = std::stoi(v);
		}

		auto result = Result::success("删除成功~");
		result.put("checkedKeys", m_menuService.getMenuListByRoleId(roleId));
		result.put("menus", m_menuService.getMenuTreeSelect(std::make_shared<SysMenu>(), getUserId()));
		return sendResult(ctx, HTTP_STATUS_OK,result);
	}

};

std::shared_ptr<MenuController> MenuController::createShared()
{
	return std::make_shared<MenuControllerImpl>();
}
