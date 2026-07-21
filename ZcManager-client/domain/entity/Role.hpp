#pragma once

#include "common/utils/Utils.h"
#include "common/config/Config.h"
#include <QString>

struct Role {
	qint32 role_id{ -1 };// INT PRIMARY KEY AUTO_INCREMENT COMMENT '菜单ID',
	QString role_name;// VARCHAR(32) NOT NULL COMMENT '角色名称',
	QString role_key;// VARCHAR(64) NOT NULL COMMENT '角色权限字符',
	quint8 role_sort{ 1 };// TINYINT NOT NULL COMMENT '显示排序',
	quint8 is_active;// TINYINT NOT NULL DEFAULT 1 COMMENT '角色状态 0-禁用 1-正常',
	quint8 is_deleted;// TINYINT NOT NULL DEFAULT 0 COMMENT '删除标志 0-已删除 1-正常',
	QString create_time;// DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
	QString create_by;//   VARCHAR(32) NULL DEFAULT NULL COMMENT '创建人',
	QString update_time;// DATETIME NULL DEFAULT NULL COMMENT '更新时间',
	QString update_by;// VARCHAR(32) NULL DEFAULT NULL COMMENT '更新人',
	QString remark;// VARCHAR(256) NULL DEFAULT NULL COMMENT '备注'

	QList<int> menuIds;	/*!此角色对应的所有菜单Id*/


	static bool isAdmin(qint32 roleId) {
		return roleId > 0 && roleId == 1;
	}

	bool isAdmin()const {
		return isAdmin(role_id);
	}
};
