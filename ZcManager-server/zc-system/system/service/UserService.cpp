#include "UserService.h"
#include "../dao/UserDao.h"
#include "../dao/RoleDao.h"
#include "../dao/DeptDao.h"
#include "../dao/UserRoleDao.h"
#include "../dao/UserPostDao.h"
#include "common/utils/PageUtils.hpp"
#include "common/exception/BusinessError.hpp"
#include "common/sql/MysqlUtils.hpp"
#include <numeric>


UserService::UserService()
    :m_userDao(UserDao::createShared())
    ,m_roleDao(RoleDao::createShared())
    ,m_deptDao(DeptDao::createShared())
    ,m_userRoleDao(UserRoleDao::createShared())
    ,m_userPostDao(UserPostDao::createShared())
{
}

UserService::~UserService()
{}

std::shared_ptr<PagingVo<SysUser>> UserService::selectUserList(const Object<SysUser>& user, int32_t page, int32_t pageSize)
{
	if (page < 1)
		page = 1;
	if (pageSize > 100)
		pageSize = 100;

	auto list = m_userDao->selectUserList(user, page, pageSize);
	return PageUtils(page, pageSize, list.second).getPageData(list.first);
}

ObjectList<SysUser> UserService::selectUserListBy(const Object<SysUser>& user)
{
	auto page = m_userDao->selectUserList(user, 1, std::numeric_limits<int32_t>::max());
	return page.first;
}

std::shared_ptr<PagingVo<SysUser>> UserService::selectAllocatedList(const Object<SysUser>& user, int32_t page, int32_t pageSize)
{
	auto list = m_userDao->selectAllocatedList(user, page, pageSize);
	return PageUtils(page, pageSize, list.second).getPageData(list.first);
}

std::shared_ptr<PagingVo<SysUser>> UserService::selectUnallocatedList(const Object<SysUser>& user, int32_t page, int32_t pageSize)
{
	auto list = m_userDao->selectUnallocatedList(user, page, pageSize);
	return PageUtils(page, pageSize, list.second).getPageData(list.first);
}

Object<SysUser> UserService::createUser(const Object<SysUser>&user)
{
	user->nick_name = m_nameService.createUserName();
	return m_userDao->createUser(user);
}

Object<SysUser> UserService::selectUserByUserName(const std::string& name)
{
    auto user = m_userDao->selectByUsername(name);
	if (user) {
        user->roles = m_roleDao->selectRoleListByUserId(*user->user_id);
		for (auto& role : user->roles) {
			user->roleIds.push_back(*role->role_id);
		}
	}
    return user;
}

Object<SysUser> UserService::selectUserByUserId(int32_t userId,const zc::mysql::PooledConnection& con)
{
	auto user = m_userDao->selectById(userId,con);
	if (!user)
		return {};

	//获取用户角色
	user->roles = m_roleDao->selectRoleListByUserId(*user->user_id,con);

	//获取用户角色id
	for (auto& role : user->roles) {
		user->roleIds.push_back(*role->role_id);
	}

	//获取第一个角色id
	if (!user->roleIds.empty()) {
		user->role_id = user->roleIds.front();
	}

	//查询部门数据
	if (user->dept_id && user->dept_id > 0) {
		user->dept = m_deptDao->selectById(*user->dept_id,con);
	}

	return user;
}

Object<SysUser> UserService::insertUser(const Object<SysUser>& user)
{
	zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());
	try {
		//插入用户
		m_userDao->insert(user, trans.getConnection());

		//获取插入的ID
		user->user_id = MysqlUtils::lastInsertId(trans.getConnection());

		//插入用户岗位关联
		insertUserPost(user, trans.getConnection());

		//插入用户角色关联
		insertUserRole(user, trans.getConnection());
		trans.commit();
		return m_userDao->selectById(*user->user_id);
	}
	catch (const std::exception& e) {
		throw BusinessError("插入用户失败");
	}
	return Object<SysUser>();
}

bool UserService::updateUser(const Object<SysUser>& user)
{
	zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());

	try {
		//删除用户角色关联
		m_userRoleDao->deleteUserRoleByUserId(*user->user_id, trans.getConnection());
		//添加用户角色关联
		insertUserRole(user, trans.getConnection());

		//删除用户岗位关联
		m_userPostDao->deleteUserPostByUserId(*user->user_id, trans.getConnection());
		//添加用户岗位关联
		insertUserPost(user, trans.getConnection());

		m_userDao->update(user, trans.getConnection());
		trans.commit();
		return true;
	}
	catch (const std::exception& e) {
		throw BusinessError(e.what());
	}
	return true;
}

bool UserService::updateLoginTime(const Object<SysUser>& user)
{
	return m_userDao->update(user);
}

bool UserService::updateUserStatus(const Object<SysUser>& user)
{
	return	m_userDao->update(user) > 0;
}

bool UserService::updatePwd(const Object<SysUser>& user)
{
	return m_userDao->updatePwd(user);
}

bool UserService::deleteUser(const Object<SysUser>& user)
{
	zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());
	try
	{
		m_userPostDao->deleteUserPostByUserId(*user->user_id, trans.getConnection());
		m_userRoleDao->deleteUserRoleByUserId(*user->user_id, trans.getConnection());
		m_userDao->deleteByIds({*user->user_id}, trans.getConnection());
		trans.commit();
		return true;
	}
	catch (const std::exception&e ) {
		throw BusinessError(e.what());
	}
	return false;
}

bool UserService::deleteUsers(const std::vector<int>& userIds)
{
	zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());
	try {
		for (auto id : userIds) {
			m_userPostDao->deleteUserPostByUserId(id,trans.getConnection());
			m_userRoleDao->deleteUserRoleByUserId(id,trans.getConnection());
		}
		m_userDao->deleteByIds(userIds,trans.getConnection());
		trans.commit();
		return true;
	}
	catch (const std::exception& e) {
		throw BusinessError(e.what());
	}
	return false;
}

bool UserService::updateAvatar(int32_t id, const std::string& path)
{
	//保存图片路径到数据库
	auto user = std::make_shared<SysUser>();
	user->user_id = id;
	user->avatar = path;

	if (!m_userDao->updateAvatar(user)) {
		return false;
	}
	return true;
}

bool UserService::insertUserPost(const Object<SysUser>& user, const zc::mysql::PooledConnection& con)
{
	std::vector<std::shared_ptr<SysUserPost>> list;
	for (auto postId : user->postIds) {
		auto userPost = std::make_shared<SysUserPost>();
		userPost->user_id = *user->user_id;
		userPost->post_id = postId;
		list.push_back(userPost);
	}
	if (list.empty()) {
		return true;
	}
	return m_userPostDao->batchUserPost(list,con);
}

bool UserService::insertUserRole(const Object<SysUser>& user, const zc::mysql::PooledConnection& con)
{
	std::vector<std::shared_ptr<SysUserRole>> list;
	for (auto roleId : user->roleIds) {
		auto userRole = std::make_shared<SysUserRole>();
		userRole->user_id = *user->user_id;
		userRole->role_id = roleId;
		list.push_back(userRole);
	}
	if (list.empty()) {
		return true;
	}
	return m_userRoleDao->batchUserRole(list, con);
}


bool UserService::checkUserNameUnique(const Object<SysUser>& user)
{
	auto userId = user->user_id.value_or(-1);
	
	auto info = m_userDao->selectByUsername(*user->user_name);
	if (info && *info->user_id != userId) {
		return false;
	}
	return true;
}

bool UserService::checkPhoneNumberUnique(const Object<SysUser>& user)
{
	auto userId = user->user_id.value_or(-1);

	auto info = m_userDao->selectByPhone(*user->phone_number);
	if (info && *info->user_id != userId) {
		return false;
	}
	return true;
}

bool UserService::checkEmailUnique(const Object<SysUser>& user)
{
	auto userId = user->user_id.value_or(-1);

	auto info = m_userDao->selectByEmail(*user->email);
	if (info && *info->user_id != userId) {
		return false;
	}
	return true;
}

std::string UserService::importUser(const ObjectList<SysUser>& list, bool isUpdateSupport, const std::string& operName)
{
	if (list.empty()) {
		throw BusinessError("导入数据不能为空！");
	}

	int successNum = 0;
	int failNum = 0;
	std::string successMsg;
	std::string failMsg;
	for (auto& user : list) {
		try {
			//验证用户是否存在
			auto u = m_userDao->selectByUsername(*user->user_name);
			//不存在插入
			if (!u) {
				user->create_by = operName;
				m_userDao->insert(user);
				successNum++;
				successMsg.append(std::format("\n {},账号 {} 导入成功", successNum, *user->user_name));
			}
			//存在则更新
			else if(isUpdateSupport) {
				user->update_by = operName;
				user->user_id = u->user_id;
				m_userDao->update(user);
				successNum++;
				successMsg.append(std::format("\n {},账号 {} 更新成功", successNum, *user->user_name));
			}
			else {
				failMsg.append(std::format("\n {},账号 {} 导入失败", failNum, *user->user_name));
				failNum++;
			}
		}
		catch (const std::exception&e) {
			failNum++;
			failMsg.append(std::format("\n {},账号 {} 导入失败:{}",failNum,*user->user_name,e.what()));
		}
	}

	if (failNum > 0) {
		failMsg.insert(0,std::format("很抱歉，导入失败！共 {} 条数据格式不正确，错误如下:",failNum));
		throw BusinessError(failMsg);
	}
	else {
		successMsg.insert(0, std::format("恭喜你，数据全部导入成功！共 {} 条,数据如下:",successNum));
	}
	return successMsg;
}

