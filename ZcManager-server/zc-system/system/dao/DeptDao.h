#pragma once

#include "common/domain/dao/BaseDao.hpp"

#include "common/domain/entity/SysDept.hpp"

class DeptDao  : public BaseDao<SysDept,DeptDao>
{
	DAO_INIT(DeptDao);
public:
	/**
	 * 查询部门列表.
	 */
	virtual ObjectList<SysDept> selectDeptList(const Object<SysDept>& dept,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;


	/**
	 * 根据ID查询所有子部门.
	 */
	virtual ObjectList<SysDept> selectChildrenDeptById(int32_t deptId,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 更新所有孩子.
	 */
	virtual bool updateDeptChildren(const ObjectList<SysDept>& children,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 修改所有父部门的状态.
	 */
	virtual bool updateParentDeptStatus(const std::vector<int32_t>& parendIds, bool isActive = true,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

public:
	virtual Object<SysDept> selectByName(const std::string& name, int32_t parentId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 是否有下级部门.
	 */
	virtual bool hasChildByDeptId(int32_t deptId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 检查部门是否存在用户.
	 */
	virtual bool checkDeptExistUser(int32_t deptId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 检查启用状态的子部门数量.
	 */
	virtual int32_t selectNormalChildrenByDeptId(int32_t deptId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
};

