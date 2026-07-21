#pragma once

#include "common/domain/model/LoginUser.hpp"
#include "common/cache/DataCache.hpp"
#include "common/constants/UserConstants.hpp"
#include "common/utils/DateTime.hpp"

#include "system/service/UserService.h"
#include "PasswordService.hpp"
#include "TokenService.hpp"


class LoginService
{
	UserService m_userService;
	TokenService m_tokenService;
	PasswordService m_pwdService;
public:

	/**
	 * 登录验证.
	 * @param username 用户名
	 * @param password 密码
	 * @return 登录成功返回token，失败返回空字符串
	 */
	std::string login(const std::string& username, const std::string& password) {
		Object<LoginUser> loginUser;
		try {
			loginUser = loadUserByUsername(username, password);
		}
		catch (const std::exception& e) {
			throw;
		}

		recordLoginInfo(loginUser->user_id);
		return m_tokenService.createToken(loginUser);
	}


	Object<LoginUser> loadUserByUsername(const std::string& username, const std::string& password) 
	{
		//根据用户名查找用户
		auto info = m_userService.selectUserByUserName(username);
		if (!info) {
			LOG_DEBUG("用户名或密码错误~");
			throw BusinessError("用户名或密码错误~");
		}

		//验证用户是否已被禁用了
		if (info->status == UserConstants::DISABLED) {
			LOG_DEBUG("用户已被禁用~");
			throw BusinessError("用户已被禁用，请联系管理员~");
		}

		//验证密码
		m_pwdService.validatePassword(*info->user_name, password, *info->password);

		//登录成功
		LOG_DEBUG("登录成功~");
		return createLoginUser(info);
	}

	Object<LoginUser> createLoginUser(const Object<SysUser>& user)
	{
		//构建登录信息
		auto loginUser = std::make_shared<LoginUser>();
		loginUser->user_id = *user->user_id;
		loginUser->dept_id = *user->dept_id;
		loginUser->loginTime = DateTime::currentDateTime().toSecsSinceEpoch();
		loginUser->user = user;

		//LocalCache::instance()->token = *info->user_name;
		//LocalCache::instance()->cacheObject(*info->user_name, loginUser);
		return loginUser;
	}

	/**
	 * 记录登录信息，主要是登录时间.
	 */
	void recordLoginInfo(int32_t userId)
	{
		auto user = std::make_shared<SysUser>();
		user->user_id = userId;
		user->login_time = DateTime::currentDateTime().toString();
		m_userService.updateLoginTime(user);
	}
};
