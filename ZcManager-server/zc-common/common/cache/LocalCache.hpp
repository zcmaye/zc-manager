#pragma once

#include <memory>
#include <any>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include "common/base/TypeTraits.h"
//#include <print>

/**
 * 缓存对象.
 */
class CacheObject {
public:
	using ptr = std::unique_ptr<CacheObject>;
public:
	CacheObject(const std::string& key, const std::any& value, const std::chrono::seconds& expire = {})
		: m_key(key)
		, m_value(value)
		, m_expire(expire)
		, m_createTime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()))
	{

	}

	void set(const std::any& value, const std::chrono::seconds& expire = {}) {
		m_value = value;
		m_expire = expire;
	}

	const std::string& key()const { return m_key; }
	const std::any& value()const { return m_value; }
	std::any& value() { return m_value; }

	/**
	 * 剩余生存时长.
	 * -1: 永不过期
	 * -2: 已过期
	 */
	long long ttl()const {
		//永不超时
		if (m_expire.count() == 0) {
			return -1;
		}
		auto expire = m_createTime + m_expire;
		auto nowSec = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
		//如果当前时间大于超时时间，则过期了
		if (nowSec > expire) {
			return -2;
		}
		return (expire - nowSec).count();
	}

	/**
	 * 设置超时时长.
	 */
	void ttl(const std::chrono::seconds& expire) {
		m_expire = expire;
	}

private:
	std::string m_key;		/*!键*/
	std::any m_value;		/*!值*/
	std::chrono::seconds m_createTime;	/*!创建时间*/
	std::chrono::seconds m_expire;		/*!超时时长*/
};


/**
 * 缓存对象池.
 */
class CachePool {
public:
	enum UpdateType {
		EXIST,		/*!存在则更新*/
		NOT_EXIST,	/*!不存在则更新*/
		ALWAYS		/*!总是更新*/
	};
public:
	CachePool(int maxSize = 1000)
		: m_maxSize(maxSize)
		, m_stop(false)
	{
		m_thread = std::thread(&CachePool::clearExpired, this);
	}

	~CachePool() {
		m_stop = true;
		m_thread.join();
	}

	static CachePool* instance() {
		static CachePool pool;
		return &pool;
	}

	/**
	 * 设置缓存.
	 *
	 * \param key		键名
	 * \param value		值
	 * \param expire	过期时长
	 * \param type	    更新类型
	 * \return	true: 成功, false: 失败
	 */
	bool set(const std::string& key, const std::any& value, const std::chrono::seconds& expire, UpdateType type = UpdateType::ALWAYS) {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_caches.size() >= m_maxSize) {
			throw std::runtime_error(std::format("key <{}>:cache is full",key));
		}
		switch (type)
		{
		case CachePool::EXIST:
		{
			auto it = m_caches.find(key);
			if (it != m_caches.end()) {
				return m_caches.emplace(key, std::make_unique<CacheObject>(key, value, expire)).second;
			}
			break;
		}
		case CachePool::NOT_EXIST:
		{
			auto it = m_caches.find(key);
			if (it == m_caches.end()) {
				return m_caches.emplace(key, std::make_unique<CacheObject>(key, value, expire)).second;
			}
			break;
		}
		case CachePool::ALWAYS:
		{
			return m_caches.emplace(key, std::make_unique<CacheObject>(key, value, expire)).second;
			break;
		}
		}
		return false;
	}


	bool set(const std::string& key, const std::any& value, UpdateType type = UpdateType::ALWAYS) {
		return set(key, value, std::chrono::seconds{}, type);
	}


	/**
	 * key是否存在.
	 */
	bool exists(const std::string& key) const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return !!m_caches.count(key);
	}

	/**
	 * 根据key获取value.
	 */
	std::any get(const std::string& key) const {
		std::lock_guard<std::mutex> lock(m_mutex);
		auto it = m_caches.find(key);
		if (it != m_caches.end()) {
			return it->second->value();
		}
		return {};
	}

	/*
	* 获取key的的剩余生存时间.
	*/
	long long ttl(const std::string& key)const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto it = m_caches.find(key);
		if (it != m_caches.end()) {
			return it->second->ttl();
		}
		return -2;
	}

	/**
	 * 删除key.
	 */
	bool del(const std::string& key) {
		std::lock_guard<std::mutex> lock(m_mutex);
		return !!m_caches.erase(key);
	}

	/**
	 * 删除多个key.
	 */
	template<typename Input>
	long long del(Input first, Input last) {
		std::lock_guard<std::mutex> lock(m_mutex);
		long long count = 0;
		for (auto it = first; it != last; it++) {
			count += m_caches.erase(*it);
		}
		return count;
	}

	/**
	 * 获取keys.
	 * user:1
	 * user:2
	 * user:3
	 * user:4
	 * example:   user:*
	 */
	template<typename Output>
	void keys(const std::string& pattern, Output output)const {
		std::lock_guard<std::mutex> lock(m_mutex);
		for (auto it = m_caches.begin(); it != m_caches.end(); it++) {
			if (isMatch(it->first, pattern)) {
				*output = it->first;
				output++;
			}
		}
	}

	std::vector<std::string> keys(const std::string& pattern)const
	{
		std::vector<std::string> result;
		keys(pattern,std::back_inserter(result));
		return result;
	}
private:
	/**
	 * 通配符匹配 *匹配任意个字符，?匹配一个字符.
	 */
	static bool isMatch(const std::string& string, const std::string& pattern) {
		auto s = string;
		auto p = pattern;
		auto allStars = [](const std::string& str, int left, int right) {
			for (int i = left; i < right; ++i) {
				if (str[i] != '*') {
					return false;
				}
			}
			return true;
			};
		auto charMatch = [](char u, char v) {
			return u == v || v == '?';
			};

		while (s.size() && p.size() && p.back() != '*') {
			if (charMatch(s.back(), p.back())) {
				s.pop_back();
				p.pop_back();
			}
			else {
				return false;
			}
		}
		if (p.empty()) {
			return s.empty();
		}

		int sIndex = 0, pIndex = 0;
		int sRecord = -1, pRecord = -1;
		while (sIndex < s.size() && pIndex < p.size()) {
			if (p[pIndex] == '*') {
				++pIndex;
				sRecord = sIndex;
				pRecord = pIndex;
			}
			else if (charMatch(s[sIndex], p[pIndex])) {
				++sIndex;
				++pIndex;
			}
			else if (sRecord != -1 && sRecord + 1 < s.size()) {
				++sRecord;
				sIndex = sRecord;
				pIndex = pRecord;
			}
			else {
				return false;
			}
		}
		return allStars(p, pIndex, p.size());
	}

	void clearExpired() {
		while (!m_stop.load()) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::lock_guard<std::mutex> lock(m_mutex);
			for (auto it = m_caches.begin(); it != m_caches.end();) {
				if (it->second->ttl() == -2) {
					//std::println("key <{}>:clearExpired", it->first);
					it = m_caches.erase(it);
				}
				else {
					++it;
				}
			}
		}
	}
private:
	std::unordered_map<std::string, CacheObject::ptr> m_caches;	/*!缓存对象池*/
	int m_maxSize;
	std::thread m_thread;
	std::atomic_bool m_stop;
	mutable std::mutex m_mutex;
};



/**
 * 本地缓存.
 */
class LocalCache {
public:
	CachePool* pool()const {
		return CachePool::instance();
	}

	/**
	 * 设置缓存对象.
	 * 
	 * \param key
	 * \param value
	 * \param expire
	 * \param type
	 */
	template<typename T, std::enable_if_t<zc::is_object_v<T>, int> = 0>
	void setCacheObject(const std::string& key, const T& value, const std::chrono::seconds& expire, CachePool::UpdateType type = CachePool::ALWAYS) {
		pool()->set(key, value, expire, type);
	}

	template<typename T,std::enable_if_t<zc::is_base_type_v<T>,int> = 0>
	void setCacheObject(const std::string& key, const T& value, const std::chrono::seconds& expire, CachePool::UpdateType type = CachePool::ALWAYS) {
		std::stringstream ss;
		ss << value;
		pool()->set(key, ss.str(), expire, type);
	}


	template<typename T>
	void setCacheObject(const std::string& key, const T& value, CachePool::UpdateType type= CachePool::ALWAYS) {
		setCacheObject(key, value, std::chrono::seconds{}, type);
	}

	/**
	 * 获取缓存对象.
	 * 
	 * \param key
	 * \return 
	 */
	template<typename T,std::enable_if_t<zc::is_object_v<T>, int> = 0>
	T getCacheObject(const std::string& key) const {
		auto value = pool()->get(key);
		if (!value.has_value()) {
			return {};
		}
		return std::any_cast<T>(value);
	}

	template<typename T,std::enable_if_t<zc::is_base_type_v<T>, int> = 0>
	T getCacheObject(const std::string& key) const {
		auto value = pool()->get(key);
		if (!value.has_value()) {
			return {};
		}
		auto val_str = std::any_cast<std::string>(value);
		std::stringstream ss(val_str);
		T t{};
		ss >> t;
		return t;
	}


	bool deleteCacheObject(const std::string& key) {
		return pool()->del(key);
	}

	int deleteCacheObject(const std::vector<std::string>& keys) {
		return pool()->del(keys.begin(), keys.end());
	}

	/**
	 * 设置过期时间.
	 *
	 * \param key
	 * \param expire
	 * \return
	 */
	bool expire(const  std::string& key, const std::chrono::seconds& expire) {
		return pool()->set(key, expire, CachePool::EXIST);
	}


	/**
	 * 获取过期时间.
	 *
	 * \param key
	 * \return
	 */
	long long getExpire(const std::string& key) const {
		return pool()->ttl(key);
	}


	/**
	 * key是否存在.
	 *
	 * \param key
	 * \return
	 */
	bool hasKey(const std::string& key)const {
		return pool()->exists(key);
	}

	/**
	 * 获取所有key.
	 *
	 * \param pattern
	 * \return
	 */
	std::vector<std::string> keys(const std::string& pattern)const {
		return pool()->keys(pattern);
	}
};





