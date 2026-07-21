#pragma once

#include "common/domain/model/LoginUser.hpp"

struct UserContext {
	/**
	 * 当前请求的登录用户.
	 */
	Object<LoginUser> loginUser;
};
