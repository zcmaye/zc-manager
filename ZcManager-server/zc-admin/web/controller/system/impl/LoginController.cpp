#include "../LoginController.h"

#include "system/service/UserService.h"
#include "system/service/PermissionService.h"
#include "system/service/PostService.h"
#include "system/service/MenuService.h"

#include "framework/service/LoginService.hpp"

class LoginControllerImpl : public LoginController{
	UserService m_userService;
	PermissionService m_permService;
	PostService m_postService;
	MenuService m_menuService;
	LoginService m_loginService;
public:
	LoginControllerImpl() {}

	int login(const HttpContextPtr& ctx) override
	{
		//获取请求体
		auto& json = ctx->json();

		std::string username;
		std::string password;
		try {
			username = json.at("username");
			password = json.at("password");
		}
		catch (const std::exception& e) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少必要参数~"));
		}

		auto result = Result::success("登录成功~");
		//登录
		auto token = m_loginService.login(username, password);
		//将token返回给客户端
		result.put("token", token);

		return sendResult(ctx, HTTP_STATUS_OK, result);
	}

	int logout(const HttpContextPtr& ctx) override
	{
		//LocalCache::instance()->deleteCacheObject(LocalCache::instance()->token);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("退出登录成功~"));
	}

	int getInfo(const HttpContextPtr& ctx) override
	{
		auto loginUser = getLoginUser();
		auto result = Result::success();
		result.put("user", loginUser->user);
		result.put("roles", m_permService.getMenuPermission(loginUser->user));
		result.put("post", m_postService.selectPostByUserId(loginUser->user_id));
		result.put("perms", m_permService.getMenuPermission(loginUser->user));

		return sendResult(ctx, HTTP_STATUS_OK, result);
	}

	int getRouters(const HttpContextPtr& ctx) override
	{
		auto menus = m_menuService.getMenuTreeByUserId(getUserId());
		auto menuTree = m_menuService.buildMenus(menus);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(menuTree));
	}

public:
	void registerRouters(hv::HttpService* router) override
	{
#if 0
		router->POST("/login", [this](const HttpContextPtr& ctx) {
			try {
				return login(ctx);
			}
			catch (const std::exception& e) {
				//如果是服务器错误
				if (auto* err = dynamic_cast<const ServiceError*>(&e)) {
					return sendResult(ctx, HTTP_STATUS_INTERNAL_SERVER_ERROR, Result::error(e.what()));
				}
				//其他错误
				else {
					return sendResult(ctx, HTTP_STATUS_OK, Result::error(e.what()));
				}
			}
			});

		router->GET("/logout", [this](const HttpContextPtr& ctx) {return logout(ctx); });
		router->GET("/getInfo", [this](const HttpContextPtr& ctx) {return getInfo(ctx); });
		router->GET("/getRouters", [this](const HttpContextPtr& ctx) {return getRouters(ctx); });
#else
		ENDPOINT(router, POST, "/login", login);
		ENDPOINT(router, GET, "/logout", logout);
		ENDPOINT(router, GET, "/getInfo", getInfo);
		ENDPOINT(router, GET, "/getRouters", getRouters);
#endif
	}
};

std::shared_ptr<LoginController> LoginController::createShared()
{
	return std::make_shared<LoginControllerImpl>();
}
