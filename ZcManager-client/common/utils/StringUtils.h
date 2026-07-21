#pragma once

#include <QString>

class StringUtils 
{
public:
	StringUtils();
	~StringUtils();

	/**
	 * 判断是不是http(s)://开头的链接.
	 */
	static bool isHttp(const QString& path);

	/**
	 * 把字符串首字母大写.
	 */
	static QString capitalize(const QString& str);


	/**
	 * 把str中的第一个befor替换成after.
	 */
	static QString replaceFirst(const QString& str,const QString& before,const QString& after);
};

