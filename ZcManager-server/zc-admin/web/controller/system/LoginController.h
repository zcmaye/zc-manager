#pragma once

#include "framework/controller/BaseController.hpp"


class LoginController : public BaseController
{
public:
	LoginController() {}
	static std::shared_ptr<LoginController> createShared();

	/**
	 * 用户登录接口.
	 */
	virtual int login(const HttpContextPtr& ctx) = 0;

	/**
	 * 用户退出登录.
	 */
	virtual int logout(const HttpContextPtr& ctx) = 0;

	/**
	 * 获取用户信息
	 */
	virtual int getInfo(const HttpContextPtr& ctx) = 0;

	/**
	 * 获取用户路由
	 */
	virtual int getRouters(const HttpContextPtr& ctx) = 0;
};
