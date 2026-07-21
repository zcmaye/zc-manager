#pragma once

#include <QString>
#include <QList>

/**
 * 部门表 dept.
 */
struct Dept {
	qint32 dept_id{0};// INT PRIMARY KEY AUTO_INCREMENT COMMENT '部门ID',
	qint32 parent_id{-1};// INT DEFAULT 0 COMMENT '父部门ID',
	QString ancestors;// VARCHAR(32) DEFAULT '' COMMENT '祖级列表',
	QString dept_name;// VARCHAR(32) NOT NULL UNIQUE  COMMENT '部门名称',
	quint8 order_num{0};// INT DEFAULT 0 COMMENT '显示顺序',
	QString leader;// VARCHAR(32) DEFAULT NULL COMMENT '负责人',
	QString phone;// VARCHAR(32) DEFAULT NULL COMMENT '联系电话',
	QString email;// VARCHAR(64) DEFAULT NULL COMMENT '邮箱',
	quint8 is_deleted{0xff};// TINYINT DEFAULT 1 COMMENT '是否删除(0-没有 1-删除了)',
	quint8 is_active{0xff};// TINYINT DEFAULT 1 COMMENT '部门状态(0-禁用 1-正常)',
	QString create_time;
	QString create_by;
	QString update_time;
	QString update_by;
	QString remark;

	/** 当前菜单的子菜单 */
	QList<std::shared_ptr<Dept>> children;
};
