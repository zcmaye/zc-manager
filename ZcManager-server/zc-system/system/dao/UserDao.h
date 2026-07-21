#pragma once

#include "common/domain/dao/BaseDao.hpp"
#include "common/domain/entity/SysUser.hpp"

class UserDao  : public BaseDao<SysUser,UserDao>
{
	DAO_INIT(UserDao)
public:
	virtual Object<SysUser> createUser(const Object<SysUser>& user, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
	virtual bool  updatePwd(const Object<SysUser>& user, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
	virtual bool  updateAvatar(const Object<SysUser>& user, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
public:
	virtual Object<SysUser> selectByUsername(const std::string& username, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
	virtual Object<SysUser> selectByPhone(const std::string& phone, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
	virtual Object<SysUser> selectByEmail(const std::string& email, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
public:
	virtual std::pair<ObjectList<SysUser>, int32_t> selectUserList(const Object<SysUser>& user, int32_t page = 1, int32_t pageSize = 10, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	virtual std::pair<ObjectList<SysUser>,int32_t> selectAllocatedList(const Object<SysUser>& user, int32_t page = 1, int32_t pageSize = 10, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
	/**
	* 查询未分配角色的用户列表.
	*/
	virtual std::pair<ObjectList<SysUser>,int32_t> selectUnallocatedList(const Object<SysUser>& user, int32_t page = 1, int32_t pageSize = 10, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
};

