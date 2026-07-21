#pragma once

#include "../entity/SysMenu.hpp"
#include "../entity/SysDept.hpp"
#include <memory>

struct TreeSelect {
	/** 节点ID */
	int32_t id;

	/** 节点名称 */
	std::string label;

	/** 子节点 */
	ObjectList<TreeSelect> children;

	TreeSelect()
		:id(-1) { }

public:	//菜单
	TreeSelect(const std::shared_ptr<SysMenu>& menu)
		:id(*menu->menu_id)
		,label(*menu->menu_name)
	{
		for (auto& m : menu->children) {
			this->children.push_back(std::make_shared<TreeSelect>(m));
		}
	}

	TreeSelect(const Object<SysDept>& dept)
		:id(*dept->dept_id)
		,label(*dept->dept_name)
	{
		for (auto& m : dept->children) {
			this->children.push_back(std::make_shared<TreeSelect>(m));
		}
	}

};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(TreeSelect,id,label,children)
