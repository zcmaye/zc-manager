#pragma once

#include "../BaseEntity.hpp"

struct SysRole : public BaseEntity {
	std::optional<int32_t> role_id;// INT PRIMARY KEY AUTO_INCREMENT COMMENT '菜单ID',
	std::optional<std::string> role_name;// VARCHAR(32) NOT NULL COMMENT '角色名称',
	std::optional<std::string> role_key;// VARCHAR(64) NOT NULL COMMENT '角色权限字符',
	std::optional<int8_t> role_sort;// TINYINT NOT NULL COMMENT '显示排序',
	std::optional<int8_t> is_active;// TINYINT NOT NULL DEFAULT 1 COMMENT '角色状态 0-禁用 1-正常',
	std::optional<int8_t> is_deleted;// TINYINT NOT NULL DEFAULT 0 COMMENT '删除标志 0-已删除 1-正常',

	std::vector<int32_t> menuIds;	/*!此角色对应的所有菜单Id*/

	static bool isAdmin(std::optional<int32_t> roleId) {
		return roleId > 0 && roleId == 1;
	}

	bool isAdmin()const {
		return isAdmin(role_id);
	}
};

MAYE_JSON_DEFINE_TYPE(SysRole, BASE_ENTITY_JSON_FIELD, role_id, role_name, role_key, role_sort, is_active, is_deleted, menuIds)
