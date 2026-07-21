#pragma once

#include "framework/controller/BaseController.hpp"

class MenuController : public BaseController
{
public:
	MenuController() {}

	static std::shared_ptr<MenuController> createShared();

	/**
	 * 获取菜单列表.
	 */
	virtual int tree(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据菜单ID获取菜单信息.
	 */
	virtual int getInfo(const HttpContextPtr& ctx) = 0;

	/**
	 * 添加菜单.
	 */
	virtual int add(const HttpContextPtr& ctx) = 0;

	/**
	 * 修改(编辑)菜单信息.
	 */
	virtual int edit(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据菜单ID删除菜单.
	 */
	virtual int remove(const HttpContextPtr& ctx) = 0;
public:
	/**
	 * 获取所有菜单选择树
	 */
	virtual int treeselect(const HttpContextPtr& ctx) = 0;

	/**
	 * 获取角色对应的菜单id.
	 */
	virtual int checkedKeys(const HttpContextPtr& ctx) = 0;

	/**
	 * 获取角色对应的菜单选择树.
	 */
	virtual int roleMenuTreeselect(const HttpContextPtr& ctx) = 0;
};

