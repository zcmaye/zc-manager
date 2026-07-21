#include "StringUtils.h"

StringUtils::StringUtils()
{}

StringUtils::~StringUtils()
{}

bool StringUtils::isHttp(const QString& path)
{
	return path.startsWith("https://") || path.startsWith("http://");
}

QString StringUtils::capitalize(const QString& str)
{
	if (str.isEmpty())
		return str;
	auto s = str;
	s.front() =  s.front().toUpper();
	return s;
}

QString StringUtils::replaceFirst(const QString& str, const QString& before, const QString& after)
{
	auto ret = str;
	auto pos = ret.indexOf(before);
	if (pos == -1) {
		return ret;
	}

	return ret.replace(pos, before.size(), after);
}
