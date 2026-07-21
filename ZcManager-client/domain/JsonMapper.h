#pragma once

#include <QJsonDocument>
#include <Qjsonobject>
#include <QJsonArray>
#include <QJsonValue>

#include "dto/PagingDto.hpp"

struct User;
struct Role;
struct Dept;
struct Post;
struct Menu;
struct RoleMenu;
struct UserPost;
struct UserRole;
struct MetaVo;
struct RouterVo;
struct TreeSelect;

/** User */
void to_json(QJsonValue& j,const User& user);

void from_json(const QJsonValue& j,User& user);

/** Role */
void to_json(QJsonValue& j,const Role& v);

void from_json(const QJsonValue& j,Role& v);

/** Dept */
void to_json(QJsonValue& j,const Dept& v);

void from_json(const QJsonValue& j,Dept& v);

/** Menu */
void to_json(QJsonValue& j,const Menu& v);

void from_json(const QJsonValue& j, Menu& v);

/** Post */
void to_json(QJsonValue& j,const Post& v);

void from_json(const QJsonValue& j, Post& v);

/** RoleMenu */
void to_json(QJsonValue& j,const RoleMenu& v);

void from_json(const QJsonValue& j, RoleMenu& v);

/** UserPost */
void to_json(QJsonValue& j,const UserPost& v);

void from_json(const QJsonValue& j, UserPost& v);

/** UserRole */
void to_json(QJsonValue& j,const UserRole& v);

void from_json(const QJsonValue& j, UserRole& v);


/** MetaVo */
void to_json(QJsonValue& j,const MetaVo& v);

void from_json(const QJsonValue& j, MetaVo& v);

/** RouterVo */
void to_json(QJsonValue& j,const RouterVo& v);

void from_json(const QJsonValue& j, RouterVo& v);

/** TreeSelect */
void to_json(QJsonValue& j,const TreeSelect& v);

void from_json(const QJsonValue& j, TreeSelect& v);

/** std::shared_ptr<T> */
template<typename T>
void to_json(QJsonValue& j, const std::shared_ptr<T>& v)
{
	to_json(j, *v);
}

template<typename T>
void from_json(const QJsonValue& j, std::shared_ptr<T>& v)
{
	if (!v) {
		v = std::make_shared<T>();
	}
	from_json(j, *v);
}


/** PagingDto */
template<typename T>
void to_json(QJsonValue& j, const PagingDto<T>& v)
{
	QJsonObject obj;
	obj["page"] = v.page;
	obj["pageSize"] = v.pageSize;
	obj["totalCount"] = v.totalCount;

	QJsonValue jitems;
	to_json(jitems, v.items);
	obj["items"] = jitems;
	j = obj;
}

template<typename T>
void from_json(const QJsonValue& j, PagingDto<T>& v)
{
	if (!j.isObject()) {
		qWarning()<<"Error: Json is not an object";
		return;
	}
	auto obj = j.toObject();
	v.page = obj["page"].toInt();
	v.pageSize = obj["pageSize"].toInt();
	v.totalCount = obj["totalCount"].toInt();
	from_json(obj["items"], v.items);
}




/** Qt Base */
template<typename T>
void to_json(QJsonValue& j, const QList<std::shared_ptr<T>>& list)
{
	QJsonArray jarray;
	for (auto& item : list) {
		QJsonValue jvalue;
		to_json(jvalue, *item);
		jarray.push_back(jvalue);
	}
	j = jarray;
}

template<typename T>
void from_json(const QJsonValue& j, QList<std::shared_ptr<T>>& list)
{
	if (!j.isArray()) {
		qDebug() << "Error: Json is not an array";
		return;
	}

	for (auto jvalue : j.toArray()) {
		auto item = std::make_shared<T>();
		from_json(jvalue,*item);
		list.push_back(item);
	}
}


template<typename T>
void to_json(QJsonValue& j, const QList<T>& list)
{
	QJsonArray jarray;
	for (auto& item : list) {
		jarray.push_back(item);
	}
	j = jarray;
}

template<typename T>
void from_json(const QJsonValue& j, QList<T>& list)
{
	if (!j.isArray()) {
		qDebug() << "Error: Json is not an array";
		return;
	}

	for (auto jvalue : j.toArray()) {
		if constexpr (std::is_integral_v<T>) {
			list.push_back(jvalue.toVariant().toLongLong());
		}
		else if constexpr (std::is_floating_point_v<T>) {
			list.push_back(jvalue.toVariant().toDouble());
		}
		else if constexpr (std::is_same_v<T, QString>) {
			list.push_back(jvalue.toVariant().toString());
		}
		else if constexpr (std::is_same_v<T, QByteArray>) {
			list.push_back(jvalue.toVariant().toByteArray());
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			list.push_back(jvalue.toVariant().toString().toStdString());
		}
		else {
			T t{};
			from_json(jvalue, t);
			list.push_back(t);
		}
	}
}

template<typename T>
void to_json(QJsonValue& j, const QSet<T>& list)
{
	QJsonArray jarray;
	for (auto& item : list) {
		jarray.push_back(item);
	}
	j = jarray;
}

template<typename T>
void from_json(const QJsonValue& j, QSet<T>& list)
{
	if (!j.isArray()) {
		qDebug() << "Error: Json is not an array";
		return;
	}

	for (auto jvalue : j.toArray()) {
		if constexpr (std::is_integral_v<T>) {
			list.insert(jvalue.toVariant().toLongLong());
		}
		else if constexpr (std::is_floating_point_v<T>) {
			list.insert(jvalue.toVariant().toDouble());
		}
		else if constexpr (std::is_same_v<T, QString>) {
			list.insert(jvalue.toVariant().toString());
		}
		else if constexpr (std::is_same_v<T, QByteArray>) {
			list.insert(jvalue.toVariant().toByteArray());
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			list.insert(jvalue.toVariant().toString().toStdString());
		}
		else {
			T t{};
			from_json(jvalue, t);
			list.insert(t);
		}
	}
}


