#pragma once

#include "framework/controller/BaseController.hpp"

/**
 * 用户修改自己的资料.
 */
class ProfileController : public BaseController
{
public:
	ProfileController() {}

	static std::shared_ptr<ProfileController> createShared();

	/**
	 * 上传头像
	 */
	virtual int avatar(const HttpContextPtr& ctx) = 0;

	/**
	 * 下载头像
	 */
	virtual int getAvatar(const HttpContextPtr& ctx) = 0;

	/**
	 * 更新资料
	 */
	virtual int updateProfile(const HttpContextPtr& ctx) = 0;

	/**
	 * 更新密码.
	 */
	virtual int updatePwd(const HttpContextPtr& ctx) = 0;
};

