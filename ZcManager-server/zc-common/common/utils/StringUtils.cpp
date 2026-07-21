#include "StringUtils.h"
#include "common/constants/Constants.hpp"
#include <sstream>

bool StringUtils::isHttp(const std::string& path)
{
	return path.starts_with(Constants::HTTP) || path.starts_with(Constants::HTTPS);
}

std::string StringUtils::capitalize(const std::string& str)
{
	if (str.empty())
		return str;
	auto s = str;
	s.front() = toupper(s.front());
	return s;
}

std::string StringUtils::replaceFirst(const std::string& str, const std::string& before, const std::string& after)
{
	auto ret = str;
	auto pos = ret.find(before);
	if (pos == std::string::npos) {
		return ret;
	}

	return ret.replace(pos, before.size(), after);
}

std::string StringUtils::replaceEach(const std::string& str, const std::string& before, const std::string& after)
{
	auto ret = str;
	size_t pos = 0;
	do {
		pos = ret.find(before, pos);
		if (pos == std::string::npos) {
			return ret;
		}
		ret.replace(pos, before.size(), after);
		pos += before.size() - 1;
	} while (true);
	return ret;
}

StringList StringUtils::split(const std::string& str, char delimiter)
{
	if (str.empty()) {
		return StringList();
	}
	StringList  list;

	std::stringstream ss(str);
	while (ss) {
		std::string str;
		std::getline(ss, str, ',');
		if (str.empty()) {
			break;
		}
		list.push_back(str);
	}
	return list;
}
