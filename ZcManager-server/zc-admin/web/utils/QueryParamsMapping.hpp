#pragma once

#include "common/base/Types.hpp"
#include "common/domain/entity/SysUser.hpp"
#include "common/domain/entity/SysMenu.hpp"
#include "common/domain/entity/SysRole.hpp"
#include "common/domain/entity/SysDept.hpp"
#include "system/domain/entity/SysPost.hpp"
#include "hv/HttpServer.h"

class QueryParamsMapping {
public:
	/*
	* 从查询中获取指定的参数值 
	*/
	static std::optional<std::string> get(const hv::QueryParams& params, const std::string&key)
	{
		if (params.empty()) {
			return std::nullopt;
		}

		auto it = params.find(key);
		if (it == params.end()) {
			return std::nullopt;
		}
		return it->second;
	}

	template<typename T>
	static const Object<T>& mapParams(const hv::QueryParams& params, const Object<T>& obj)
	{
		if (auto v = get(params,"page")) {
			obj->params.page = std::stoi(*v);
		}
		if (auto v = get(params,"pageSize")) {
			obj->params.pageSize = std::stoi(*v);
		}
		if (auto v = get(params,"begin_time")) {
			obj->params.begTime = *v + " 00:00:00";
		}
		if (auto v = get(params,"end_time")) {
			obj->params.endTime = *v + " 23:59:59";
		}
		return obj;
	}

	template<typename T>
	static Object<T> mapTo(const hv::QueryParams& params);

	template<>
	static Object<SysUser> mapTo(const hv::QueryParams& params)
	{
		//获取参数
		auto user = std::make_shared<SysUser>();
		if (auto v = get(params,"user_name")) {
			user->user_name = v;
		}
		if (auto v = get(params,"nick_name")) {
			user->nick_name = v;
		}
		if (auto v = get(params,"phone_number")) {
			user->phone_number = v;
		}
		if (auto v = get(params,"sex")) {
			user->sex = std::stoi(*v);
		}
		if (auto v = get(params,"status")) {
			user->status = std::stoi(*v);
		}
		return mapParams(params, user);
	}

	template<>
	static Object<SysRole> mapTo(const hv::QueryParams& params)
	{
		//获取参数
		auto role = std::make_shared<SysRole>();
		if (auto v = get(params,"role_name")) {
			role->role_name = v;
		}
		if (auto v = get(params,"role_key")) {
			role->role_key = v;
		}
		if (auto v = get(params,"is_active")) {
			role->is_active = std::stoi(*v);
		}
		return mapParams(params,role);
	}

	template<>
	static Object<SysPost> mapTo(const hv::QueryParams& params)
	{
		auto post = std::make_shared<SysPost>();
		if (auto v = get(params,"post_name")) {
			post->post_name = v;
		}
		if (auto v = get(params,"post_code")) {
			post->post_code = v;
		}
		if (auto v = get(params,"is_active")) {
			post->is_active = std::stoi(*v);
		}
		return mapParams(params,post);
	}

	template<>
	static Object<SysMenu> mapTo(const hv::QueryParams& params)
	{
		//获取参数
		auto menu = std::make_shared<SysMenu>();
		if (auto v = get(params,"menu_name")) {
			menu->menu_name = v;
		}
		if (auto v = get(params,"is_active")) {
			menu->is_active = std::stoi(*v);
		}
		return mapParams(params,menu);
	}

	template<>
	static Object<SysDept> mapTo(const hv::QueryParams& params)
	{
		auto dept = std::make_shared<SysDept>();
		if (auto v = get(params,"dept_name")) {
			dept->dept_name = v;
		}
		if (auto v = get(params,"is_active")) {
			dept->is_active = std::stoi(*v);
		}
		return mapParams(params,dept);
	}
};