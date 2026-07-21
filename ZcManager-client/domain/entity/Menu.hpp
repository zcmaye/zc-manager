#pragma once

#include <QString>
#include <QList>


struct Menu {
	qint32 menu_id{0};
	QString menu_name;
	qint32 parent_id;
	qint32 order_num{0};
	QString path;
	QString component;
	quint8 is_frame{(quint8)-1};
	QString menu_type;
	quint8 is_visible{(quint8)-1};
	quint8 is_active{(quint8)-1};
	QString perms;
	QString icon;
	QString create_time;
	QString create_by;
	QString update_time;
	QString update_by;
	QString remark;

	/** 当前菜单的子菜单 */
	QList<std::shared_ptr<Menu>> children;
};
