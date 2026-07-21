#pragma once

#include "framework/controller/BaseController.hpp"
#include "system/service/UserService.h"

class RoleController : public BaseController
{
public:
	RoleController() {}

	static std::shared_ptr<RoleController> createShared();

	/**
	 * 获取角色列表.
	 */
	virtual int list(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据角色ID获取角色信息.
	 */
	virtual int getInfo(const HttpContextPtr& ctx) = 0;

	/**
	 * 添加角色.
	 */
	virtual int add(const HttpContextPtr& ctx) = 0;

	/**
	 * 修改(编辑)角色信息.
	 */
	virtual int edit(const HttpContextPtr& ctx) = 0;

	/**
	 * 修改角色状态(启用/禁用).
	 */
	virtual int updateStatus(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据角色ID删除角色.
	 */
	virtual int remove(const HttpContextPtr& ctx) = 0;


	virtual int options(const HttpContextPtr& ctx) = 0;

	/**
	 * 查询已分配的用户角色列表
	 */
	virtual int allocatedList(const HttpContextPtr& ctx) = 0;

	/**
	 * 查询未分配的用户角色列表
	 */
	virtual int unallocatedList(const HttpContextPtr& ctx) = 0;


	/**
	 * 取消用户授权.
	 */
	virtual int cancelAuthUser(const HttpContextPtr& ctx) = 0;


	/**
	 * 批量取消用户授权.
	 */
	virtual int cancelAuthUserAll(const HttpContextPtr& ctx) = 0;


	/*
	* 批量选择用户授权
	*/
	virtual int selectAuthUserAll(const HttpContextPtr& ctx) = 0;
};

