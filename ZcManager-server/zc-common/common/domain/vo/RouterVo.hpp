#pragma once

#include "common/base/Types.hpp"
#include "MetaVo.hpp"

/**
 * 路由.
 */
struct RouterVo {
	/** 名称 */
	std::string name;

	/** 地址(路径) */
	std::string path;

	/** 是否隐藏,当为true是，就不会在侧边栏显示了 */
	bool hidden;

	/** 重定向地址，当为noRedirect时，在侧边栏中就不能被选中了 */
	std::string redirect;

	/** 组件地址 */
	std::string component;

	/** 元信息 */
	Object<MetaVo> meta;

	/** 子路由 */
	ObjectList<RouterVo> children;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RouterVo, name, path, hidden, redirect, component, meta, children)
