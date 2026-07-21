#include "RoleService.h"
#include "../dao/RoleDao.h"
#include "../dao/RoleMenuDao.h"
#include "../dao/UserRoleDao.h"

#include "common/sql/MysqlUtils.hpp"
#include "common/domain/entity/SysUser.hpp"
#include "common/utils/PageUtils.hpp"
#include "common/exception/BusinessError.hpp"

RoleService::RoleService()
	: m_roleDao(RoleDao::createShared())
	, m_roleMenuDao(RoleMenuDao::createShared())
	, m_userRoleDao(UserRoleDao::createShared())
{
}

RoleService::~RoleService()
{}

std::shared_ptr<PagingVo<SysRole>> RoleService::selectRoleList(const std::shared_ptr<SysRole>& role, int32_t page, int32_t pageSize)
{
	auto list = m_roleDao->selectRoleList(role, page, pageSize);
	return PageUtils(page, pageSize).getPageData(list);
}

ObjectList<SysRole> RoleService::selectRoleAll(const zc::mysql::PooledConnection& con)
{
	return m_roleDao->selectRoleList(std::make_shared<SysRole>(), 1, 1000, con);
}

Object<SysRole> RoleService::selectRoleById(int32_t roleId)
{
	return m_roleDao->selectById(roleId);
}

std::set<std::string> RoleService::selectRolePermissionByUserId(int32_t userId)
{
	auto roles = m_roleDao->selectRoleListByUserId(userId);
	std::set<std::string> ret;
	for (auto& role : roles) {
		ret.insert(*role->role_key);
	}
	return ret;
}

bool RoleService::updateRole(const std::shared_ptr<SysRole>& role)
{
;
	zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());

	try {
		//更新角色
		m_roleDao->update(role, trans.getConnection());
		//更新角色菜单关联
		//--先删除
		m_roleMenuDao->deleteByIds({ *role->role_id }, trans.getConnection());
		//--再添加
		m_roleMenuDao->batchInsertRoleMenu(*role->role_id, role->menuIds, trans.getConnection());
		trans.commit();
	}
	catch (const std::exception& e) {
		throw BusinessError(std::format("更新角色失败~:{}",e.what()));
	}
	return true;
}

bool RoleService::updateRoleStatus(const std::shared_ptr<SysRole>& role)
{
	return m_roleDao->updateRoleStatus(role);
}

bool RoleService::deleteRole(const std::shared_ptr<SysRole>& role)
{
	zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());

	//检查是否允许操作角色
	checkRoleAllowed(*role->role_id);

	try {
		//获取角色已经分配给用户的个数
		if (m_userRoleDao->countUserRoleByRoleId(*role->role_id, trans.getConnection()) != 0) {
			throw std::runtime_error("已分配，不能删除~");
		}

		//删除角色
		m_roleDao->deleteByIds({ *role->role_id }, trans.getConnection());

		//删除角色对应的菜单
		m_roleMenuDao->deleteByIds({ *role->role_id }, trans.getConnection());
		trans.commit();
	}
	catch (const std::exception& e) {
		throw BusinessError(std::format("删除角色失败~:{}",e.what()));
	}
	return true;
}

bool RoleService::deleteRoles(const std::vector<int>& roleIds)
{
	zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());
	try {
		for (int roleId : roleIds) {
			auto role = std::make_shared<SysRole>();
			role->role_id = roleId;
			//检查是否允许操作角色
			checkRoleAllowed(*role->role_id);
			//获取角色已经分配给用户的个数
			if (m_userRoleDao->countUserRoleByRoleId(*role->role_id, trans.getConnection()) != 0) {
				throw BusinessError("已分配，不能删除~");
			}
		}

		//删除角色
		m_roleDao->deleteByIds(roleIds,trans.getConnection());

		//删除角色对应的菜单
		m_roleMenuDao->deleteByIds(roleIds, trans.getConnection());
		trans.commit();
	}
	catch (const std::exception& e) {
		throw BusinessError(std::format("删除角色失败~:{}",e.what()));
	}
	return true;
}

bool RoleService::insertRole(const std::shared_ptr<SysRole>& role)
{
	zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());

	try
	{
		//插入角色
		m_roleDao->insert(role, trans.getConnection());

		//获取刚插入的角色的id
		auto lastId = MysqlUtils::lastInsertId(trans.getConnection());

		//插入角色对应的菜单
		role->role_id = lastId;

		if (!role->menuIds.empty()) {
			m_roleMenuDao->batchInsertRoleMenu(*role->role_id, role->menuIds, trans.getConnection());
		}

		trans.commit();
		return true;
	}
	catch (const std::exception& e) {
		throw BusinessError("角色插入失败~");
	}
	return false;
}

bool RoleService::deletedAuthUser(const std::shared_ptr<SysUserRole>& userRole)
{
	return m_userRoleDao->deletedUserRoleInfo(userRole);
}

bool RoleService::deletedAuthUsers(int32_t roleId, const std::vector<int32_t> userIds)
{
	return m_userRoleDao->deletedUserRoleInfos(roleId,userIds);
}

bool RoleService::insertAuthUsers(int32_t roleId, const std::vector<int32_t> userIds)
{
	std::vector<std::shared_ptr<SysUserRole>> list;
	for (auto userId : userIds) {
		auto ur = std::make_shared<SysUserRole>();
		ur->role_id = roleId;
		ur->user_id = userId;
		list.push_back(ur);
	}
	return m_userRoleDao->batchUserRole(list);
}

void RoleService::checkRoleAllowed(int32_t roleId) const
{
	if (SysRole::isAdmin(roleId)) {
		throw std::runtime_error("不允许操作管理员角色");
	}
}

bool RoleService::checkRoleNameUnique(const std::shared_ptr<SysRole>& role)
{
	auto roleId = role->role_id.value_or(-1);

	auto info = m_roleDao->selectRoleByName(*role->role_name);
	if(info && info->role_id != roleId){
		return false;
	}

	return true;
}

bool RoleService::checkRoleKeyUnique(const std::shared_ptr<SysRole>& role)
{
	auto roleId = role->role_id.value_or(-1);

	auto info = m_roleDao->selectRoleByKey(*role->role_key);
	if(info && info->role_id != roleId){
		return false;
	}

	return true;
}
