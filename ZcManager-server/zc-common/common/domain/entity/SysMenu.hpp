#pragma once

#include "../BaseEntity.hpp"

struct SysMenu : public BaseEntity {
	std::optional<int32_t> menu_id;
	std::optional<std::string> menu_name;
	std::optional<int32_t> parent_id;
	std::optional<int32_t> order_num;
	std::optional<std::string> path;
	std::optional<std::string> component;
	std::optional<int8_t> is_frame;
	std::optional<std::string> menu_type;
	std::optional<int8_t> is_visible;
	std::optional<int8_t> is_active;
	std::optional<std::string> perms;
	std::optional<std::string> icon;
	/** 当前菜单的子菜单 */
	ObjectList<SysMenu> children;
};

MAYE_JSON_DEFINE_TYPE(SysMenu,BASE_ENTITY_JSON_FIELD,menu_id,menu_name,parent_id,order_num,path,component,is_frame,menu_type,is_visible,is_active,
	perms,icon,children)
