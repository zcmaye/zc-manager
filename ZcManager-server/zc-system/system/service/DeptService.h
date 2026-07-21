#pragma once

#include "common/domain/entity/SysDept.hpp"
#include "common/domain/vo/TreeSelect.hpp"

class DeptDao;

class DeptService
{
public:
	DeptService();
	~DeptService();

	ObjectList<SysDept> selectDeptList(const Object<SysDept>& dept);

	Object<SysDept> selectDeptById(int32_t deptId);

	/**
	 * 是否存在下级部门.
	 */
	bool hasChildByDeptId(int32_t deptId);

	/**
	 * 部门中是否存在用户.
	 */
	bool checkDeptExistUser(int32_t deptId);

	int32_t selectNormalChildrenByDeptId(int32_t deptId);

	/**
	 * 获取部门管理部门树
	 */
	ObjectList<SysDept> getDeptTree(const Object<SysDept>& dept = std::make_shared<SysDept>());

	/**
	 * 获取部门选择树
	 */
	ObjectList<TreeSelect> getDeptTreeSelect(const Object<SysDept>& dept);

	/**
	 * 删除部门.
	 */
	bool deleteDeptById(int32_t deptId);

	/**
	 * 插入部门.
	 */
	bool insertDept(const Object<SysDept>& dept);

	/**
	 * 修改部门.
	 */
	bool updateDept(const Object<SysDept>& dept);
private:
	/**
	 * 更新所有子部门.
	 */
	void updateDeptChildren(int32_t deptId,const std::string& newAncestors,const std::string& oldAncestors);

	/**
	 * 修改所有父部门的状态.
	 */
	void updateParentDeptStatus(const Object<SysDept>& dept, bool isActive = true);
private:
	/**
	 * 把depts中的所有parent_id为parentId的孩子都放到他的父节点中.
	 */
	ObjectList<SysDept> getChildPerms(const ObjectList<SysDept>& depts,int32_t parentId);

	/**
	 * 递归获取parent的孩子.
	 */
	void recursionFn(const ObjectList<SysDept>& depts, const Object<SysDept>& parent);

	/**
	 * 获取孩子列表.
	 * @param depts 在这个列表中查找孩子啊
	 * @param parent 查找这个的孩子
	 * @return 返回parent的所有孩子列表
	 */
	ObjectList<SysDept>  getChildList(const ObjectList<SysDept>& depts, const Object<SysDept>& parent);

	/**
	 * 判断是否有孩子.
	 */
	bool hasChild(const ObjectList<SysDept>& depts, const Object<SysDept>& parent);
private:
	/**
	 * 构建部门选择树.
	 */
	ObjectList<TreeSelect> buildDeptTreeSelect(const ObjectList<SysDept>& depts);

	/**
	 * 构建部门树
	 */
	ObjectList<SysDept>buildDeptTree(const ObjectList<SysDept>& depts);
public:
	bool checkDeptNameUnique(const Object<SysDept>& dept)const;
private:
	std::shared_ptr<DeptDao> m_deptDao;
};

