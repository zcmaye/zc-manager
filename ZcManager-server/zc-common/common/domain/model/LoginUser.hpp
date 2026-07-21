#pragma once

#include "common/json/njson.hpp"
#include "common/domain/entity/SysUser.hpp"

/**
 * 登录用户.
 */
struct LoginUser {

	/**
	 * 用户ID.
	 */
	int32_t user_id;

	/**
	 * 部门id.
	 */
	int32_t dept_id;

	/**
	 * 用户唯一标识.
	 */
	std::string token;

    /**
	* 登录时间
	*/
	int64_t loginTime;

    /**
	* 过期时间
	*/
	int64_t expireTime;

	/**
	 * 登录IP地址.
	 */
	std::string ipaddr;

	/**
	 * 登录地点.
	 */
	std::string loginLocation;

	/**
	 *  浏览器类型
	 */
	std::string browser;

	/**
	 * 操作系统.
	 */
	std::string os;

	/**
	 * 权限列表.
	 */
	std::set<std::string> permissions;

	/**
	 * 用户信息.
	 */
	Object<SysUser> user;
};

MAYE_JSON_DEFINE_TYPE(LoginUser,
	user_id,dept_id,token,loginTime,expireTime,ipaddr,loginLocation,browser,os,permissions,user)


