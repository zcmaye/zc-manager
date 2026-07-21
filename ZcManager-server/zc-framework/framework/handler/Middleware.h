#pragma once

#include "hv/HttpService.h"

class Middleware
{
public:
	/**
	 * 用户认证中间件.
	 */
	static int auth(const HttpContextPtr& ctx);

	/**
	 * 后处理中间件
	 */
	static int postprocessor(const HttpContextPtr& ctx);
};
