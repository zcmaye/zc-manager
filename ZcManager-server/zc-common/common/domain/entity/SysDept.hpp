#pragma once

#include "../BaseEntity.hpp"

/**
 * 部门表 dept.
 */
struct SysDept : public BaseEntity{
	std::optional<int32_t> dept_id;// INT PRIMARY KEY AUTO_INCREMENT COMMENT '部门ID',
	std::optional<int32_t> parent_id;// INT DEFAULT 0 COMMENT '父部门ID',
	std::optional<std::string> ancestors;// VARCHAR(32) DEFAULT '' COMMENT '祖级列表',
	std::optional<std::string> dept_name;// VARCHAR(32) NOT NULL UNIQUE  COMMENT '部门名称',
	std::optional<int8_t> order_num;// INT DEFAULT 0 COMMENT '显示顺序',
	std::optional<std::string> leader;// VARCHAR(32) DEFAULT NULL COMMENT '负责人',
	std::optional<std::string> phone;// VARCHAR(32) DEFAULT NULL COMMENT '联系电话',
	std::optional<std::string> email;// VARCHAR(64) DEFAULT NULL COMMENT '邮箱',
	std::optional<int8_t> is_deleted;// TINYINT DEFAULT 1 COMMENT '是否删除(0-没有 1-删除了)',
	std::optional<int8_t> is_active;// TINYINT DEFAULT 1 COMMENT '部门状态(0-禁用 1-正常)',

	/** 当前菜单的子菜单 */
	ObjectList<SysDept> children;
};

MAYE_JSON_DEFINE_TYPE(SysDept,BASE_ENTITY_JSON_FIELD, dept_id, parent_id, ancestors, dept_name, order_num, leader, phone, email, is_deleted, is_active, children);
