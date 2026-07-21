#pragma once

#include "../entity/Menu.hpp"
#include "../entity/Dept.hpp"
#include <QString>
#include <QList>
#include <memory>

struct TreeSelect {
	/** 节点ID */
	qint32 id;

	/** 节点名称 */
	QString label;

	/** 子节点 */
	QList<std::shared_ptr<TreeSelect>> children;

	TreeSelect()
		:id(-1) { }

public:	//菜单
	TreeSelect(const std::shared_ptr<Menu>& menu)
		:id(menu->menu_id)
		,label(menu->menu_name)
	{
		for (auto& m : menu->children) {
			this->children.append(std::make_shared<TreeSelect>(m));
		}
	}

	TreeSelect(const std::shared_ptr<Dept>& dept)
		:id(dept->dept_id)
		,label(dept->dept_name)
	{
		for (auto& m : dept->children) {
			this->children.append(std::make_shared<TreeSelect>(m));
		}
	}

};

