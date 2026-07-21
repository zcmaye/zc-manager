#pragma once

#include "framework/controller/BaseController.hpp"

class UserController : public BaseController
{
public:
	UserController() {}

	static std::shared_ptr<UserController> createShared();

	/**
	 * 获取用户列表.
	 */
	virtual int list(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据用户ID获取用户信息.
	 */
	virtual int getInfo(const HttpContextPtr& ctx) = 0;

	/**
	 * 添加用户.
	 */
	virtual int add(const HttpContextPtr& ctx) = 0;

	/**
	 * 修改(编辑)用户信息.
	 */
	virtual int edit(const HttpContextPtr& ctx) = 0;


	/**
	 * 修改用户状态(启用/禁用).
	 */
	virtual int updateStatus(const HttpContextPtr& ctx) = 0;

	/**
	 * 修改用户密码.
	 */
	virtual int resetPwd(const HttpContextPtr& ctx) = 0;


	/**
	 * 根据用户ID删除用户.
	 */
	virtual int remove(const HttpContextPtr& ctx) = 0;

	/**
	 * 导入用户.
	 */
	virtual int import_(const HttpContextPtr& ctx) = 0;

	/**
	 * 导出用户.
	 */
	virtual int export_(const HttpContextPtr& ctx) = 0;
};

