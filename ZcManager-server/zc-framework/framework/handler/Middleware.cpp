#include "Middleware.h"
#include "../service/TokenService.hpp"
#include "common/cache/DataCache.hpp"
#include "../result/Result.hpp"
#include "../context/UserContext.hpp"

/**
 * 要跳过验证的路由.
 */
static std::set<std::string> paths = { "/login", "/register"};

int Middleware::auth(const HttpContextPtr& ctx)
{
	std::printf("path: %s\n", ctx->path().c_str());

	//跳过路由
	//if (ctx->path() == "/login" || ctx->path() == "/register") {
	if(paths.contains(ctx->path())) {
		//继续处理
		return HTTP_STATUS_NEXT;
	}

	TokenService tokenService;
	//获取登录用户
	auto loginUser = tokenService.getLoginUser(ctx);
	//如果存在用户，则放行
	if (loginUser) {
		//验证token
		tokenService.verifyToken(loginUser);

		//创建用户上下文
		auto context = new UserContext;
		context->loginUser = loginUser;
		//将用户上下文，存放到请求上下文中
		ctx->userdata  = context;
		return HTTP_STATUS_NEXT;
	}

	ctx->sendJson(Result::error("请先登录~").toJson());
	return HTTP_STATUS_UNAUTHORIZED;
}

int Middleware::postprocessor(const HttpContextPtr& ctx)
{
	if (!ctx->userdata) {
		return HTTP_STATUS_NEXT;
	}

	//释放用户上下文
	auto context = (UserContext*)ctx->userdata;
	if (context) {
		delete context;
		return HTTP_STATUS_NEXT;
	}

	return HTTP_STATUS_NEXT;
}
