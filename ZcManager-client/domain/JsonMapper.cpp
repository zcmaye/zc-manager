#include "JsonMapper.h"
#include "entity/User.hpp"
#include "entity/Role.hpp"
#include "entity/Dept.hpp"
#include "entity/Post.hpp"
#include "entity/Menu.hpp"
#include "entity/RoleMenu.hpp"
#include "entity/UserPost.hpp"
#include "entity/UserRole.hpp"
#include "vo/MetaVo.hpp"
#include "vo/RouterVo.hpp"
#include "vo/TreeSelect.hpp"


template<typename T>
static inline QJsonValue to_json(const QList<T>& list)
{
	QJsonValue j;
	to_json(j, list);
	return j;
}

template<typename T>
static inline QList<T> from_json(const QJsonValue& j)
{
	if (!j.isArray()) {
		qDebug()<<"JsonMapper::from_json: not an array";
		return {};
	}
	QList<T> list;
	from_json(j, list);
	return list;
}



/** User */
void to_json(QJsonValue& j, const User& user)
{
	QJsonObject obj;
	obj["user_id"] = user.user_id;
	obj["dept_id"] = user.dept_id;
	obj["user_name"] = user.user_name;
	obj["nick_name"] = user.nick_name;
	obj["password"] = user.password;
	obj["phone_number"] = user.phone_number;
	obj["email"] = user.email;
	obj["avatar"] = user.avatar;
	obj["sex"] = user.sex;
	obj["birthday"] = user.birthday;
	obj["status"] = user.status;
	obj["is_deleted"] = user.is_deleted;
	obj["login_time"] = user.login_time;
	obj["create_time"] = user.create_time;
	obj["create_by"] = user.create_by;
	obj["update_time"] = user.update_time;
	obj["update_by"] = user.update_by;
	obj["remark"] = user.remark;

	obj["role_id"] = user.role_id;
	obj["roles"] = to_json(user.roles);
	obj["roleIds"] = to_json(user.roleIds);
	obj["postIds"] = to_json(user.postIds);

	j = obj;
}

void from_json(const QJsonValue& j, User& user)
{
	if (!j.isObject()) {
		qWarning()<<"JsonMapper::from_json: not an object";
		return;
	}
	user.user_id = j["user_id"].toInt();
	user.dept_id = j["dept_id"].toInt();
	user.user_name = j["user_name"].toString();
	user.nick_name = j["nick_name"].toString();
	user.password = j["password"].toString();
	user.phone_number = j["phone_number"].toString();
	user.email = j["email"].toString();
	user.avatar = j["avatar"].toString();
	user.sex = j["sex"].toInt();
	user.birthday = j["birthday"].toString();
	user.status = j["status"].toInt();
	user.is_deleted = j["is_deleted"].toInt();
	user.login_time = j["login_time"].toString();
	user.create_time = j["create_time"].toString();
	user.create_by = j["create_by"].toString();
	user.update_time = j["update_time"].toString();
	user.update_by = j["update_by"].toString();
	user.remark = j["remark"].toString();
	user.role_id = j["role_id"].toInt();
	user.roles = from_json<std::shared_ptr<Role>>(j["roles"]);
	user.roleIds = from_json<qint32>(j["roleIds"]);
	user.postIds = from_json<qint32>(j["postIds"]);
}

/** Role */
void to_json(QJsonValue& j, const Role& v)
{
	QJsonObject obj;
	obj["role_id"] = v.role_id;
	obj["role_name"] = v.role_name;
	obj["role_key"] = v.role_key;
	obj["role_sort"] = v.role_sort;
	obj["is_active"] = v.is_active;
	obj["is_deleted"] = v.is_deleted;

	obj["create_time"] = v.create_time;
	obj["create_by"] = v.create_by;
	obj["update_time"] = v.update_time;
	obj["update_by"] = v.update_by;
	obj["remark"] = v.remark;

	obj["menuIds"] = to_json(v.menuIds);

	j = obj;
}

void from_json(const QJsonValue& j, Role& v)
{
	if (!j.isObject()) {
		qWarning()<<"JsonMapper::from_json: not an object";
		return;
	}

	v.role_id = j["role_id"].toInt();
	v.role_name = j["role_name"].toString();
	v.role_key = j["role_key"].toString();
	v.role_sort = j["role_sort"].toInt();
	v.is_active = j["is_active"].toInt();
	v.is_deleted = j["is_deleted"].toInt();
	v.create_time = j["create_time"].toString();
	v.create_by = j["create_by"].toString();
	v.update_time = j["update_time"].toString();
	v.update_by = j["update_by"].toString();
	v.remark = j["remark"].toString();
	v.menuIds = from_json<qint32>(j["menuIds"]);
}

/** Dept */
void to_json(QJsonValue& j, const Dept& v) {
	QJsonObject obj;
	obj["dept_id"] = v.dept_id;
	obj["parent_id"] = v.parent_id;
	obj["ancestors"] = v.ancestors;
	obj["dept_name"] = v.dept_name;
	obj["order_num"] = v.order_num;
	obj["leader"] = v.leader;
	obj["phone"] = v.phone;
	obj["email"] = v.email;
	obj["is_deleted"] = v.is_deleted;
	obj["is_active"] = v.is_active;
	obj["create_time"] = v.create_time;
	obj["create_by"] = v.create_by;
	obj["update_time"] = v.update_time;
	obj["update_by"] = v.update_by;
	obj["remark"] = v.remark;

	obj["children"] = to_json(v.children);

	j = obj;
}

void from_json(const QJsonValue& j, Dept& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}

	v.dept_id = j["dept_id"].toInt();
	v.parent_id = j["parent_id"].toInt();
	v.ancestors = j["ancestors"].toString();
	v.dept_name = j["dept_name"].toString();
	v.order_num = j["order_num"].toInt();
	v.leader = j["leader"].toString();
	v.phone = j["phone"].toString();
	v.email = j["email"].toString();
	v.is_deleted = j["is_deleted"].toInt();
	v.is_active = j["is_active"].toInt();
	v.create_time = j["create_time"].toString();
	v.create_by = j["create_by"].toString();
	v.update_time = j["update_time"].toString();
	v.update_by = j["update_by"].toString();
	v.remark = j["remark"].toString();
	v.children = from_json<std::shared_ptr<Dept>>(j["children"]);
}


/** Menu */
void to_json(QJsonValue& j, const Menu& v) {
	QJsonObject obj;
	obj["menu_id"] = v.menu_id;
	obj["menu_name"] = v.menu_name;
	obj["parent_id"] = v.parent_id;
	obj["order_num"] = v.order_num;
	obj["path"] = v.path;
	obj["component"] = v.component;
	obj["is_frame"] = v.is_frame;
	obj["is_active"] = v.is_active;
	obj["menu_type"] = v.menu_type;
	obj["is_visible"] = v.is_visible;
	obj["perms"] = v.perms;
	obj["icon"] = v.icon;
	obj["create_time"] = v.create_time;
	obj["create_by"] = v.create_by;
	obj["update_time"] = v.update_time;
	obj["update_by"] = v.update_by;
	obj["remark"] = v.remark;

	obj["children"] = to_json(v.children);

	j = obj;
}

void from_json(const QJsonValue& j, Menu& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}
	v.menu_id = j["menu_id"].toInt();
	v.menu_name = j["menu_name"].toString();
	v.parent_id = j["parent_id"].toInt();
	v.order_num = j["order_num"].toInt();
	v.path = j["path"].toString();
	v.component = j["component"].toString();
	v.is_frame = j["is_frame"].toInt();
	v.is_active = j["is_active"].toInt();
	v.menu_type = j["menu_type"].toString();
	v.is_visible = j["is_visible"].toInt();
	v.perms = j["perms"].toString();
	v.icon = j["icon"].toString();
	v.create_time = j["create_time"].toString();
	v.create_by = j["create_by"].toString();
	v.update_time = j["update_time"].toString();
	v.update_by = j["update_by"].toString();
	v.remark = j["remark"].toString();
	v.children = from_json<std::shared_ptr<Menu>>(j["children"]);
}

/** Post */
void to_json(QJsonValue& j, const Post& v) {
	QJsonObject obj;
	obj["post_id"] = v.post_id;
	obj["post_code"] = v.post_code;
	obj["post_name"] = v.post_name;
	obj["post_sort"] = v.post_sort;
	obj["is_active"] = v.is_active;
	obj["create_time"] = v.create_time;
	obj["create_by"] = v.create_by;
	obj["update_time"] = v.update_time;
	obj["update_by"] = v.update_by;
	obj["remark"] = v.remark;

	j = obj;
}

void from_json(const QJsonValue& j, Post& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}

	v.post_id = j["post_id"].toInt();
	v.post_code = j["post_code"].toString();
	v.post_name = j["post_name"].toString();
	v.post_sort = j["post_sort"].toInt();
	v.is_active = j["is_active"].toInt();
	v.create_time = j["create_time"].toString();
	v.create_by = j["create_by"].toString();
	v.update_time = j["update_time"].toString();
	v.update_by = j["update_by"].toString();
	v.remark = j["remark"].toString();
}

/** RoleMenu */
void to_json(QJsonValue& j, const RoleMenu& v) {
	QJsonObject obj;
	obj["role_id"] = v.role_id;
	obj["menu_id"] = v.menu_id;
	j = obj;
}

void from_json(const QJsonValue& j, RoleMenu& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}
	v.role_id = j["role_id"].toInt();
	v.menu_id = j["menu_id"].toInt();
}

/** UserPost */
void to_json(QJsonValue& j, const UserPost& v) {
	QJsonObject obj;
	obj["user_id"] = v.user_id;
	obj["post_id"] = v.post_id;
	j = obj;
}

void from_json(const QJsonValue& j, UserPost& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}

	v.user_id = j["user_id"].toInt();
	v.post_id = j["post_id"].toInt();
}

/** UserRole */
void to_json(QJsonValue& j, const UserRole& v) {
	QJsonObject obj;
	obj["user_id"] = v.user_id;
	obj["role_id"] = v.role_id;
	j = obj;
}

void from_json(const QJsonValue& j, UserRole& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}

	v.user_id = j["user_id"].toInt();
	v.role_id = j["role_id"].toInt();
}


/** MetaVo */
void to_json(QJsonValue& j, const MetaVo& v) {
	QJsonObject obj;
	obj["title"] = v.title;
	obj["icon"] = v.icon;
	obj["link"] = v.link;
	j = obj;
}

void from_json(const QJsonValue& j, MetaVo& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}
	v.title = j["title"].toString();
	v.icon = j["icon"].toString();
	v.link = j["link"].toString();
}

/** RouterVo */
void to_json(QJsonValue& j, const RouterVo& v) {
	QJsonObject obj;
	obj["name"] = v.name;
	obj["path"] = v.path;
	obj["hidden"] = v.hidden;
	obj["redirect"] = v.redirect;
	obj["component"] = v.component;

	if (v.meta) {
		QJsonValue jmeta;
		to_json(jmeta, *v.meta);
		obj["meta"] = jmeta;
	}
	else {
		obj["meta"] = {};
	}

	obj["children"] = to_json(v.children);
	j = obj;
}

void from_json(const QJsonValue& j, RouterVo& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}
	v.name = j["name"].toString();
	v.path = j["path"].toString();
	v.hidden = j["hidden"].toBool();
	v.redirect = j["redirect"].toString();
	v.component = j["component"].toString();

	v.meta = std::make_shared<MetaVo>();
	from_json(j["meta"], *v.meta);

	v.children = from_json<std::shared_ptr<RouterVo>>(j["children"]);
}

/** TreeSelect */
void to_json(QJsonValue& j, const TreeSelect& v) {
	QJsonObject obj;
	obj["id"] = v.id;
	obj["label"] = v.label;

	obj["children"] = to_json(v.children);
	j = obj;
}

void from_json(const QJsonValue& j, TreeSelect& v) {
	if (!j.isObject()) {
		qWarning() << "JsonMapper::from_json: not an object";
		return;
	}
	v.id = j["id"].toInt();
	v.label= j["label"].toString();
	v.children = from_json<std::shared_ptr<TreeSelect>>(j["children"]);
}
