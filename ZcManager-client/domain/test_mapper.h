#include "JsonMapper.h"
#include "common/json/QJson.hpp"

#include "entity/User.hpp"
#include "entity/Role.hpp"

inline void test_user()
{
	auto user = std::make_shared<User>();
	user->user_id = 1001;
	user->user_name = "admin";
	user->nick_name = "admin";
	user->password = "123456";
	user->phone_number = "123456789";
	user->email = "123456789@qq.com";
	user->sex = 1;
	user->birthday = QDate(2020, 1, 1).toString("yyyy-MM-dd");
	user->create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	user->create_by = "admin";

	user->roleIds = { 1,3,5,7,9 };
	user->postIds = { 1,3,5,7,9 };
	user->roles = {};

	QJsonValue jv;
	to_json(jv, *user);
	qDebug() << QJsonDocument(jv.toObject()).toJson();


	User _user;
	from_json(jv, _user);
}

inline void test_role()
{
	auto v = std::make_shared<Role>();
	v->role_id = 1001;
	v->role_name = "admin";
	v->role_key = "admin";
	v->role_sort  = 123;

	v->create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	v->create_by = "admin";

	v->menuIds = { 1,3,5,7,9 };

	QJsonValue jv;
	to_json(jv, *v);
	qDebug() << QJsonDocument(jv.toObject()).toJson();

	Role _v;
	from_json(jv, _v);
}

inline void test_QJson()
{
	auto user = std::make_shared<User>();
	user->user_id = 1001;
	user->user_name = "admin";
	user->nick_name = "admin";
	user->password = "123456";
	user->phone_number = "123456789";
	user->email = "123456789@qq.com";
	user->sex = 1;
	user->birthday = QDate(2020, 1, 1).toString("yyyy-MM-dd");
	user->create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	user->create_by = "admin";

	user->roleIds = { 1,3,5,7,9 };
	user->postIds = { 1,3,5,7,9 };
	user->roles = {};

	QJson json(user);
	QJson json1(*user);

	qDebug() << json.dump();
	qDebug() << json1.dump();

	User _user = json;
	std::shared_ptr<User> p_user = json;

	auto t_user =  QJson(user).cast<User>();
	auto t_user_p =  QJson(user).cast<std::shared_ptr<User>>();

	QJson json2 = {
		{"name","maye"},
		{"age",12},
	};

	auto t = json2["name"];
	auto t1 = json2["age"];

	auto json_str = json2.dump();
	auto j = QJson::parse(json_str);
	qDebug() << j.dump();
}

inline void test()
{
	test_QJson();
}
