#pragma once

#include "common/base/Types.hpp"

class StringUtils
{
public:
	/**
	 * 判断是不是http(s)://开头的链接.
	 */
	static bool isHttp(const std::string& path);

	/**
	 * 把字符串首字母大写.
	 */
	static std::string capitalize(const std::string& str);


	/**
	 * 把str中的第一个befor替换成after.
	 */
	static std::string replaceFirst(const std::string& str, const std::string& before, const std::string& after);

	/**
	 * 把str中的每个befor替换成after.
	 */
	static std::string replaceEach(const std::string& str, const std::string& before, const std::string& after);

public:
	/**
	 * 按照指定的分隔符分割字符串.
	 */
	static StringList split(const std::string& str, char delimiter = ',');

	template<typename T>
	static std::vector<T> split(const std::string& str, char delimiter = ',')
	{
		StringList list = split(str, delimiter);
		std::vector<T> resultList;
		std::transform(list.begin(), list.end(), std::back_inserter(resultList), [](const std::string& str) {
			std::stringstream ss(str);
			T t;
			ss >> t;
			return t;
			});
		return resultList;
	}
};
