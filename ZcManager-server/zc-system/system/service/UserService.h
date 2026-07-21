#pragma once

#include "common/base/Singleton.hpp"
#include "common/domain/entity/SysUser.hpp"
#include "common/domain/vo/PagingVo.hpp"
#include "UserNameService.h"
#include "common/sql/ConnectionPool.h"

class UserDao;
class RoleDao;
class DeptDao;
class UserRoleDao;
class UserPostDao;

class UserService 
{
public:
	UserService();
	~UserService();


	/**
	 * 查询用户列表.
	 */
	std::shared_ptr<PagingVo<SysUser>> selectUserList(const Object<SysUser>& user, int32_t page = 1, int32_t pageSize = 10);
	ObjectList<SysUser> selectUserListBy(const Object<SysUser>& user);
	/**
	 * 查询已分配角色的用户列表.
	 */
	std::shared_ptr<PagingVo<SysUser>> selectAllocatedList(const Object<SysUser>& user, int32_t page = 1, int32_t pageSize = 10);
	/**
	 * 查询未分配角色的用户列表.
	 */
	std::shared_ptr<PagingVo<SysUser>> selectUnallocatedList(const Object<SysUser>& user, int32_t page = 1, int32_t pageSize = 10);


	/**
	 * 创建用户(用户自己注册专用).
	 */
	Object<SysUser> createUser(const Object<SysUser>& user);
	Object<SysUser> selectUserByUserName(const std::string& name);
	Object<SysUser> selectUserByUserId(int32_t userId, const zc::mysql::PooledConnection& con);

	Object<SysUser> insertUser(const Object<SysUser>& user);
	bool  updateUser(const Object<SysUser>& user);
	bool  updateLoginTime(const Object<SysUser>& user);
	bool  updateUserStatus(const Object<SysUser>& user);
	bool  updatePwd(const Object<SysUser>& user);
	bool  deleteUser(const Object<SysUser>& user);
	bool  deleteUsers(const std::vector<int>& userIds);
public:
	bool updateAvatar(int32_t id,const std::string& path);

	/**
	 * 插入用户岗位关联数据.
	 */
	bool insertUserPost(const Object<SysUser>& user,const zc::mysql::PooledConnection& con);
	/**
	 * 插入用户角色关联数据.
	 */
	bool insertUserRole(const Object<SysUser>& user,const zc::mysql::PooledConnection& con);
public:
	bool checkUserNameUnique(const Object<SysUser>& user);
	bool checkPhoneNumberUnique(const Object<SysUser>& user);
	bool checkEmailUnique(const Object<SysUser>& user);
private:
	UserNameService m_nameService;
	std::shared_ptr<UserDao> m_userDao;
	std::shared_ptr<RoleDao> m_roleDao;
	std::shared_ptr<DeptDao> m_deptDao;
	std::shared_ptr<UserRoleDao> m_userRoleDao;
	std::shared_ptr<UserPostDao> m_userPostDao;

public:
	/**
	 * 导入用户数据.
	 * @param list 用户数据列表
	 * @param isUpdateSupport 是否支持更新，如果已经存在，则更新数据
	 * @param operName 操作用户
	 */
	std::string importUser(const ObjectList<SysUser>& list,bool isUpdateSupport,const std::string& operName);
};

