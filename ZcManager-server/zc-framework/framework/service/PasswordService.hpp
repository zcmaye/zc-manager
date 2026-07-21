#pragma once

#include "common/utils/EncryptUtils.hpp"
#include "common/exception/BusinessError.hpp"
#include "common/cache/DataCache.hpp"

class PasswordService{
	DataCache m_dataCache;
	const int MAX_ERR_CNT = 3;
	const int LOCK_TIME = 3;	//分钟
public:
	std::string getCacheKey(const std::string& username) const {
		return "pw_err_cnt:" + username;
	}

	void validatePassword(const std::string& username, const std::string& rawPassword, const std::string& hashedPassword) {

		//获取密码错误次数
		auto errCnt = m_dataCache.getCacheObject<int>(getCacheKey(username));
		if (errCnt >= MAX_ERR_CNT) {
			throw BusinessError(std::format("用户密码错误{}次，请{}分钟后再试", MAX_ERR_CNT, LOCK_TIME));
		}

		if (!EncryptUtils::matchesPassword(rawPassword, hashedPassword)) {
			m_dataCache.setCacheObject(getCacheKey(username), errCnt += 1, std::chrono::minutes(LOCK_TIME));
			throw BusinessError("用户名或密码错误");
		}
		else {
			clearLoginErrCnt(username);
		}
	}

	void clearLoginErrCnt(const std::string& username) {
		m_dataCache.deleteCacheObject(getCacheKey(username));
	}
};
