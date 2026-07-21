#pragma once

#include <QString>

/**
 * 路由的元信息.
 */
struct MetaVo
{
	/** 设置改路由在导航栏和面包屑中展示的名字 */
	QString title;

	/** 路由对应的图标，对应路径:/Resource/icons/svg */
	QString icon;

	/** 内连接地址(http(s)://开头) */
	QString link;

	MetaVo() = default;
	MetaVo(const QString& title, const QString& icon, const QString& link = {})
		:title(title)
		,icon(icon)
	{
		if (link.startsWith("https://") || link.startsWith("http://")) {
			this->link = link;
		}
	}
};
