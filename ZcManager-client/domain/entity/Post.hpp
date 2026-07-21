#pragma once

#include <QString>
#include <QList>

/**
 * 岗位表 post.
 */
struct Post {
	qint32 post_id{ 0 };
	QString post_code;
	QString post_name;
	qint32 post_sort{ 0 };
	quint8 is_active{ (quint8)-1 };
	QString create_time;
	QString create_by;
	QString update_time;
	QString update_by;
	QString remark;
};

