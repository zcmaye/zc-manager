#pragma once

#include "common/json/njson.hpp"
#include <string>

/**
 * 路由的元信息.
 */
struct MetaVo
{
	/** 设置改路由在导航栏和面包屑中展示的名字 */
	std::string title;

	/** 路由对应的图标，对应路径:/Resource/icons/svg */
	std::string icon;

	/** 内连接地址(http(s)://开头) */
	std::string link;

	MetaVo() = default;
	MetaVo(const std::string& title, const std::string& icon, const std::string& link = {})
		:title(title)
		,icon(icon)
	{
		if (link.starts_with("https://") || link.starts_with("http://")) {
			this->link = link;
		}
	}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MetaVo, title, icon,link)
