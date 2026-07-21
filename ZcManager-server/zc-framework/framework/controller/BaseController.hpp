#pragma once

#include "common/domain/model/LoginUser.hpp"
#include "common/cache/DataCache.hpp"
#include "common/exception/ServiceError.hpp"
#include "common/exception/BusinessError.hpp"
#include "hv/HttpService.h"
#include "../result/Result.hpp"
#include "../context/UserContext.hpp"
#include <memory>


/**
 * 打印日志.
 */
#define LOG_DEBUG(format, ...) printf("[DEBUG] %s:%d : " format "\n",__FILE__,__LINE__, ##__VA_ARGS__)

/**
 * 添加端点.
 * @param PATH 请求路径
 * @param FUNC 处理函数
 */
#define ENDPOINT(ROUTER,METHOD,PATH,FUNC)\
			(ROUTER)->METHOD(PATH, [this](const HttpContextPtr& ctx) {\
			try {\
				this->m_ctx = ctx;\
				return FUNC(ctx);\
			}\
			catch (const std::exception& e) {\
				if (auto* err = dynamic_cast<const ServiceError*>(&e)) {\
					return sendResult(ctx, HTTP_STATUS_INTERNAL_SERVER_ERROR, Result::error(e.what()));\
				}\
				else {\
					return sendResult(ctx, HTTP_STATUS_OK, Result::error(e.what()));\
				}\
			}\
			})


class BaseController
{
protected:
	HttpContextPtr m_ctx;
public:
	BaseController() {}
	virtual ~BaseController() {}

	/**
	 * 获取当前操作的用户名.
	 */
	std::string getUsername()const {
		return *getLoginUser()->user->user_name;
	}

	/**
	 * 获取当前操作的用户ID.
	 */
	int32_t getUserId()const {
		return getLoginUser()->user_id;
	}

	/**
	 * 获取当前登录用户.
	 */
	Object<LoginUser> getLoginUser() const {
		try
		{
			auto userContext = static_cast<UserContext*>(m_ctx->userdata);
			if (!userContext) {
				throw std::runtime_error("用户未登录");
			}
			else if (!userContext->loginUser) {
				throw std::runtime_error("用户未登录");
			}
			return userContext->loginUser;
		}
		catch (const std::exception& e) {
			throw std::runtime_error("用户未登录");
		}
	}
public:
	int sendResult(const HttpContextPtr& ctx, http_status status, const Result& result) {
		ctx->sendJson(result.toJson());
		return status;
	}
public:
	/**
	 * 子类必须实现，以注册路由.
	 */
	virtual void registerRouters(hv::HttpService* router) = 0;
};
