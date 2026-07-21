#pragma once

#include "common/base/TypeTraits.h"
#include "nlohmann/json.hpp"
#include "framework/context/ContextHolder.h"
#include <sstream>

#ifdef USE_REDIS_CACHE
#include "sw/redis++/redis++.h"

class RedisCache {
	std::shared_ptr<sw::redis::Redis> m_redis;
public:
	RedisCache(const std::shared_ptr<sw::redis::Redis>& redis = ContextHolder::instance()->redis)
		:m_redis(redis)
	{

	}


	/**
	 * 设置缓存对象(基本类型，整型，浮点型，字符串).
	 */
	template<typename T,std::enable_if_t<zc::is_base_type_v<T>,int> = 0>
	void setCacheObject(const std::string& key, const T& value, const std::chrono::seconds& expire, sw::redis::UpdateType type = sw::redis::UpdateType::ALWAYS)
	{
		//如果值是字符串类型，则直接设置
		if constexpr (zc::is_string_v<T>) {
			m_redis->set(key, value, expire, type);
		}
		//否则要转成字符串
		else {
			std::stringstream ss;
			ss << value;
			m_redis->set(key, ss.str(), expire, type);
		}
	}

	/**
	 * 设置缓存对象(基本类型，整型，浮点型，字符串).
	 */
	template<typename T, std::enable_if_t<!zc::is_base_type_v<T> && !zc::is_object_v<T>, int> = 0>
	void setCacheObject(const std::string& key, const T& value, const std::chrono::seconds& expire, sw::redis::UpdateType type = sw::redis::UpdateType::ALWAYS)
	{
		nlohmann::json j = value;
		m_redis->set(key, j.dump(), expire, type);
	}

	/**
	 * 设置缓存对象(基本类型，整型，浮点型，字符串).
	 */
	template<typename T,std::enable_if_t<zc::is_object_v<T>,int> = 0>
	void setCacheObject(const std::string& key, const T& value, const std::chrono::seconds& expire, sw::redis::UpdateType type = sw::redis::UpdateType::ALWAYS)
	{
		setCacheObject(key, *value, expire, type);
	}

	template<typename T,std::enable_if_t<zc::is_object_v<T>,int> = 0>
	void setCacheObject(const std::string& key, const T& value, sw::redis::UpdateType type = sw::redis::UpdateType::ALWAYS)
	{
		setCacheObject(key, value, std::chrono::seconds{}, type);
	}


	/**
	 * 获取缓存对象(基本类型，整型，浮点型，字符串).
	 */
	template<typename T,std::enable_if_t<zc::is_base_type_v<T>,int> = 0>
	T getCacheObject(const std::string& key)const {
		auto val_str = m_redis->get(key);
		if (!val_str) {
			return T{};	
		}

		if constexpr (zc::is_string_v<T>) {
			return *val_str;
		}
		std::stringstream ss(*val_str);
		T val;
		ss >> val;
		return  val;
	}

	/**
	 * 获取缓存对象(非基本类型).
	 */
	template<typename T, std::enable_if_t<!zc::is_base_type_v<T> && !zc::is_object_v<T>, int> = 0>
	T getCacheObject(const std::string& key)const
	{
		auto val_str = m_redis->get(key);
		if (!val_str) {
			return T{};
		}

		return nlohmann::json::parse(*val_str);
	}

	/**
	 * 获取缓存对象(std::shared_ptr<T>,T不是基本类型).
	 */
	template<typename T, std::enable_if_t<zc::is_object_v<T>, int> = 0>
	T getCacheObject(const std::string& key)const
	{
		auto val_str = m_redis->get(key);
		if (!val_str) {
			return T{};
		}

		typename T::element_type val = nlohmann::json::parse(*val_str);
		return std::make_shared<typename T::element_type>(val);
	}

	bool deleteCacheObject(const std::string& key) {
		return m_redis->del(key);
	}

	int deleteCacheObject(const std::vector<std::string>& keys) {
		return m_redis->del(keys.begin(), keys.end());
	}


	bool expire(const std::string& key, const std::chrono::seconds& expire) {
		return m_redis->expire(key, expire);
	}

	long long getExpire(const std::string& key) const{
		return m_redis->ttl(key);
	}

	bool hasKey(const std::string& key) const{
		return !!m_redis->exists(key);
	}

	std::vector<std::string> keys(const std::string& pattern) const {
		std::vector<std::string> result;
		m_redis->keys(pattern,std::back_inserter(result));
		return result;
	}
};
#endif
