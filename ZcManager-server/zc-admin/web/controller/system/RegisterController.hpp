#pragma once

#include "framework/controller/BaseController.hpp"
#include "system/service/UserService.h"
#include "common/utils/EncryptUtils.hpp"

class RegisterController : public BaseController
{
	UserService m_userService;
public:
	RegisterController() {}

	static std::shared_ptr<RegisterController> createShared() {
		return std::make_shared<RegisterController>();
	}

	void registerRouters(hv::HttpService* router) override {
		ENDPOINT(router, POST, "/register", reg);
	}


	int reg(const HttpContextPtr& ctx) {
		auto& json = ctx->json();

		auto user = std::make_shared<SysUser>();

		try {
			user->user_name = json.at("username");
			user->password = json.at("password");
		}
		catch (const std::exception& e) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少参数"));
		}

		std::string msg;
		//检查参数
		if (user->user_name->size() < 2 || user->user_name->size() > 20) {
			msg = "用户名必须在2~20个字符之间";
		}
		else if (user->password->size() < 5 || user->password->size() > 20) {
			msg = "密码必须在5~20个字符之间";
		}
		//检查用户是否存在
		else if (!m_userService.checkUserNameUnique(user)) {
			msg = std::format("注册用户'{}'失败，用户名已存在~ ",*user->user_name);
		}
		else {
			//注册用户
			try {
				//加密密码
				user->password = EncryptUtils::encrpytPassword(*user->password);
				//创建用户
				auto newUser = m_userService.createUser(user);
				//TODO:给用户发送唯一token
				return sendResult(ctx, HTTP_STATUS_OK, Result::success());
			}
			catch (const std::exception& e) {
				LOG_DEBUG("%s", e.what());
				return sendResult(ctx, HTTP_STATUS_OK, Result::error("注册失败,请联系管理员~"));
			}
		}
		return sendResult(ctx, HTTP_STATUS_OK, Result::error(msg));
	}
};
