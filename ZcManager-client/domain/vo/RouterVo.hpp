#pragma once

#include "MetaVo.hpp"
#include <QString>
#include <QList>


/**
 * 路由.
 */
struct RouterVo {
	/** 名称 */
	QString name;

	/** 地址(路径) */
	QString path;

	/** 是否隐藏,当为true是，就不会在侧边栏显示了 */
	bool hidden;

	/** 重定向地址，当为noRedirect时，在侧边栏中就不能被选中了 */
	QString redirect;

	/** 组件地址 */
	QString component;

	/** 元信息 */
	std::shared_ptr<MetaVo> meta;

	/** 子路由 */
	QList<std::shared_ptr<RouterVo>> children;
};
