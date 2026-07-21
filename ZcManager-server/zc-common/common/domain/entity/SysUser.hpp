#pragma once

#include "../BaseEntity.hpp"
#include "SysRole.hpp"
#include "SysDept.hpp"

struct SysUser : public BaseEntity{
	std::optional<int32_t>  user_id;
	std::optional<int32_t>  dept_id;
	std::optional<std::string>  user_name;
	std::optional<std::string>  nick_name;
	std::optional<std::string>  password;
	std::optional<std::string>  phone_number;
	std::optional<std::string>  email;
	std::optional<std::string>  avatar;
	std::optional<uint8_t>  sex;
	std::optional<std::string>  birthday;
	std::optional<uint8_t>  status;
	std::optional<uint8_t>  is_deleted;
	std::optional<std::string>  login_time;

	/** 用户所属角色 */
	std::optional<int32_t>  role_id;

	/** 用户所属部门 */
	Object<SysDept> dept;

	/** 用户所属所有角色 */
	ObjectList<SysRole> roles;

	/** 用户所属角色Id数组*/
	std::vector<int32_t> roleIds;

	/** 用户所属所有岗位id数组 */
	std::vector<int32_t> postIds;

	/**
	 * 是否是管理员.
	 */
	static bool isAdmin(std::optional<int32_t> userId) {
		return userId > 0 && userId == 1;
	}

	bool isAdmin()const {
		return isAdmin(user_id);
	}
};



MAYE_JSON_DEFINE_TYPE(SysUser, BASE_ENTITY_JSON_FIELD, user_id,dept_id,user_name,nick_name,password,phone_number,email,avatar,
	sex,birthday,status,is_deleted,login_time,role_id,dept,roles,roleIds,postIds)