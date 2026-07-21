#pragma once


#include "common/utils/Utils.h"
#include "common/config/Config.h"
#include <QString>

struct Role;

struct User{
	qint32 user_id{0};
	qint32 dept_id{0};
	QString user_name;
	QString nick_name;
	QString password;
	QString phone_number;
	QString email;
	QString avatar;
	quint8 sex{2};
	QString birthday;
	qint8 status;
	qint8 is_deleted;
	QString login_time;
	QString create_time;
	QString create_by;
	QString update_time;
	QString update_by;
	QString remark;

	/** 用户所属角色 */
	qint32 role_id{0};

	/** 用户所属所有角色 */
	QList<std::shared_ptr<Role>> roles;

	/** 用户所属角色Id数组*/
	QList<qint32> roleIds;

	/** 用户所属所有岗位id数组 */
	QList<qint32> postIds;

	/**
	 * 是否是管理员.
	 */
	static bool isAdmin(quint32 userId) {
		return userId > 0 && userId == 1;
	}

	bool isAdmin()const {
		return isAdmin(user_id);
	}

	QPixmap _pixmap;
	QPixmap _tmp_pixmap;
	QPixmap avatarPixmap(bool isRect = false)
	{
		if (avatar.isEmpty()) {
			_pixmap.load(":/Resource/images/avatar.jpg");
		}
		if (_pixmap.isNull()) {
			_pixmap.load(":/Resource/images/avatar.jpg");
		}
		if (!isRect)
			return Utils::toRoundPixamp(_pixmap);
		else
			return _pixmap;
	}

public:
	inline QString sexToString()const {
		return sex == 0 ? "女" : (sex == 1 ? "男" : "未知");
	}

	inline QString statusToString()const {
		return status == 0 ? "禁用" : "正常";
	}
};
