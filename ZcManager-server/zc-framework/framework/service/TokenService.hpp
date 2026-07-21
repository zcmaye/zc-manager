#pragma once

#include "common/domain/model/LoginUser.hpp"
#include "common/cache/DataCache.hpp"
#include "common/constants/UserConstants.hpp"
#include "common/constants/CacheConstants.hpp"
#include "common/constants/Constants.hpp"

#include "common/utils/uuid/uuid.hpp"

class TokenService {
	DataCache m_dataCache;
	std::random_device m_rd;
	std::mt19937 m_eng;

	const char* AUTH_HEADER = "Authorization";
	std::chrono::minutes EXPIRE_TIME{ 30 };		/*!过期时长*/
	long long  MINUTE_TEN{ 5 * 60 };			/*!刷新token最大时长*/
public:
	TokenService() 
		: m_eng(m_rd()) 
	{}


	/**
	 * 获取登录用户.
	 */
	Object<LoginUser> getLoginUser(const HttpContextPtr& ctx) {
		//获取token
		auto token = getToken(ctx);
		//如果token为空，则返回空
		if (token.empty()) {
			return nullptr;
		}
		//根据token获取登录的用户
		auto tokenKey = getTokenKey(token);
		try {
			return m_dataCache.getCacheObject<Object<LoginUser>>(tokenKey);
		}
		catch (const std::exception& e) {
			//throw BusinessError("获取用户信息异常");
			return nullptr;
		}
	}


	/**
	 * 设置用户身份信息.
	 */
	void setLoginUser(const Object<LoginUser>& loginUser) {
		if (loginUser && !loginUser->token.empty()) {
			refreshToken(loginUser);
		}
	}

	/**
	 * 删除用户身份信息.
	 */
	void delLoginUser(const std::string& token) {
		if (!token.empty()) {
			auto key = getTokenKey(token);
			m_dataCache.deleteCacheObject(key);
		}
	}


	/**
	 * 从请求中获取token.
	 */
	std::string getToken(const HttpContextPtr& ctx) {
		//获取token
		auto token = ctx->header(AUTH_HEADER);
		//去掉Bearer admin
		if (!token.empty() && token.starts_with(Constants::TOKEN_PREFIX)) {
			token = token.substr(strlen(Constants::TOKEN_PREFIX));
		}
		return token;
	}

	/**
	 * 创建token.
	 */
	std::string createToken(const Object<LoginUser>& loginUser) {
		loginUser->token = make_uuid_string();	//token必须随机生成，而且不能重复
		refreshToken(loginUser);
		return loginUser->token;
	}

	/**
	 * 验证令牌有效期.
	 */
	void verifyToken(const Object<LoginUser>& loginUser) {
		auto expireTime = loginUser->expireTime;
		auto currentTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		//判断是否需要刷新token,距离过期时间小于5分钟，则刷新token
		if (expireTime - currentTime < MINUTE_TEN) {
			refreshToken(loginUser);
		}
	}

	/**
	 * 刷新令牌.
	 */
	void refreshToken(const Object<LoginUser>& loginUser) {
		//获取登录事件和过期事件
		loginUser->loginTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		loginUser->expireTime = (std::chrono::seconds(loginUser->loginTime)  + EXPIRE_TIME).count();
		
		auto tokenKey = getTokenKey(loginUser->token);
		m_dataCache.setCacheObject(tokenKey, loginUser, std::chrono::seconds(EXPIRE_TIME));
	}

	/**
	 * 获取缓存key.
	 */
	std::string getTokenKey(const std::string& uuid) {
		return CacheConstants::LOGIN_TOKEN_KEY + uuid;
	}


	/**
	 * 随机生成uuid字符串.
	 */
	std::string make_uuid_string() {
		uuids::uuid_random_generator gen(&m_eng);
		return uuids::to_string(gen());
	}
};
