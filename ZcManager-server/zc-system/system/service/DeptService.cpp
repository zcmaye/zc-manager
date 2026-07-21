#include "DeptService.h"
#include "../dao/DeptDao.h"
#include "common/domain/entity/SysUser.hpp"
#include "common/constants/UserConstants.hpp"
#include "common/utils/StringUtils.h"

DeptService::DeptService()
	:m_deptDao(DeptDao::createShared())
{}

DeptService::~DeptService()
{}

ObjectList<SysDept> DeptService::selectDeptList(const Object<SysDept>&dept)
{
    return m_deptDao->selectDeptList(dept);
}

Object<SysDept> DeptService::selectDeptById(int32_t deptId)
{
    return m_deptDao->selectById(deptId);
}

bool DeptService::hasChildByDeptId(int32_t deptId)
{
    return m_deptDao->hasChildByDeptId(deptId);
}

bool DeptService::checkDeptExistUser(int32_t deptId)
{
    return m_deptDao->checkDeptExistUser(deptId);
}

int32_t DeptService::selectNormalChildrenByDeptId(int32_t deptId)
{
    return m_deptDao->selectNormalChildrenByDeptId(deptId);
}

ObjectList<SysDept> DeptService::getDeptTree(const Object<SysDept>& dept)
{
    //查询部门
    auto depts = m_deptDao->selectDeptList(dept);
    //定义排除列表
    ObjectList<SysDept> excludeList;
    //获取部门的子部门
    for (auto& m : depts) {
		m->children =  getChildPerms(depts, *m->dept_id);
        excludeList.insert(excludeList.end(), m->children.begin(), m->children.end());
    }
    //把所有是子部门的删除掉
    auto delIt = std::remove_if(depts.begin(), depts.end(), [excludeList](const Object<SysDept>& m) {
            return std::find(excludeList.begin(), excludeList.end(), m) != excludeList.end();
        });
    depts.erase(delIt, depts.end());
    return  depts;
}

ObjectList<TreeSelect> DeptService::getDeptTreeSelect(const Object<SysDept>& dept)
{
    auto depts = selectDeptList(dept);
    return buildDeptTreeSelect(depts);
}


bool DeptService::deleteDeptById(int32_t deptId)
{
    return m_deptDao->deleteByIds({ deptId });
}

bool DeptService::insertDept(const Object<SysDept>& dept)
{
    if (!checkDeptNameUnique(dept)) {
        throw std::runtime_error("部门名称已存在~");
    }

    //判断父部门是否禁用，禁用了禁止插入
    auto parentDept = m_deptDao->selectById(*dept->parent_id);
    if (!parentDept) {
        throw std::runtime_error("没有选择父部门或父部门不存在~");
    }
    //禁用了
    if (parentDept->is_active == 0) {
        throw std::runtime_error("父部门停用，不允许新增~");
    }

    //祖父列表
	dept->ancestors = *parentDept->ancestors + "," + std::to_string(*dept->parent_id);

    return m_deptDao->insert(dept);
}

bool DeptService::updateDept(const Object<SysDept>& dept)
{
    //查一下
    //-- 父部门
    auto parentDept = m_deptDao->selectById(*dept->parent_id);
    if (!parentDept) {
        throw std::runtime_error("没有选择父部门或父部门不存在~");
    }
    //-- 本部门
    auto selfDept = m_deptDao->selectById(*dept->dept_id);
	if (!selfDept) {
        throw std::runtime_error("没有找到当前部门，或许是部门ID不正确?");
    }

    //如果修改了父部门，需要更新一下所有的子部门
    if (dept->parent_id != selfDept->parent_id) {
        auto newAncestors = *parentDept->ancestors + "," + std::to_string(*parentDept->dept_id);
        auto& oldAncesstors = selfDept->ancestors;
        dept->ancestors = newAncestors;
        //更新所有子部门
        updateDeptChildren(*dept->dept_id, newAncestors, *oldAncesstors);
    }

    //如果该部门是启用状态，则启用所有的上级部门
    if (dept->is_active == UserConstants::ENABLED && dept->ancestors.has_value()) {
        updateParentDeptStatus(dept);
    }
    
    return m_deptDao->update(dept);
}

void DeptService::updateDeptChildren(int32_t deptId, const std::string& newAncestors, const std::string& oldAncestors)
{
    //查询所有子部门
    auto children = m_deptDao->selectChildrenDeptById(deptId);
    if (children.empty()) {
        return;
    }

    for (auto& child : children) {
		child->ancestors = StringUtils::replaceFirst(*child->ancestors, oldAncestors, newAncestors);
    }
    m_deptDao->updateDeptChildren(children);
}

void DeptService::updateParentDeptStatus(const Object<SysDept>& dept, bool isActive)
{
    //把祖父列表中的所有id都转成整型数组
	auto parendIds = StringUtils::split(*dept->ancestors);

    std::vector<int32_t> ids;
    std::transform(parendIds.begin(), parendIds.end(), std::back_inserter(ids), [](const std::string& str) {
            return std::stoi(str);
        });
    m_deptDao->updateParentDeptStatus(ids, isActive);
}

ObjectList<SysDept> DeptService::getChildPerms(const ObjectList<SysDept>& depts, int32_t parentId)
{
    ObjectList<SysDept> returnList;
    for (auto& m : depts) {
        //如果m是parenId的孩子
        if (m->parent_id == parentId) {
            recursionFn(depts, m);
            returnList.push_back(m);
        }
    }
    return returnList;
}

void DeptService::recursionFn(const ObjectList<SysDept>& depts, const Object<SysDept>& parent)
{
    //拿到parent的Child列表
    auto childList = getChildList(depts, parent);
    for (auto& child : childList) {
        if (hasChild(depts, child)) {
            recursionFn(depts, child);
        }
    }
    parent->children = childList;
}

ObjectList<SysDept> DeptService::getChildList(const ObjectList<SysDept>& depts, const Object<SysDept>& parent)
{
    ObjectList<SysDept> returnList;
    for (auto& m : depts) {
        if (m->parent_id == parent->dept_id) {
            returnList.push_back(m);
        }
    }
    return returnList;
}

bool DeptService::hasChild(const ObjectList<SysDept>& depts, const Object<SysDept>& parent)
{
    for (auto& m : depts) {
        if (m->parent_id == parent->dept_id) {
            return true;
        }
    }
    return false;
}

ObjectList<TreeSelect> DeptService::buildDeptTreeSelect(const ObjectList<SysDept>& depts)
{
	auto deptTree = buildDeptTree(depts);

    ObjectList<TreeSelect> treeSelects;
	for (auto& m : deptTree) {
        treeSelects.push_back(std::make_shared<TreeSelect>(m));
    }
    return treeSelects;
}

ObjectList<SysDept> DeptService::buildDeptTree(const ObjectList<SysDept>& depts)
{
    ObjectList<SysDept> returnList = depts;
    //定义排除列表
    ObjectList<SysDept> excludeList;
    //获取菜单的子菜单
    for (auto& m : returnList) {
		m->children =  getChildPerms(returnList, *m->dept_id);
        excludeList.insert(excludeList.end(), m->children.begin(), m->children.end());
    }
    //把所有是子菜单的删除掉
    auto delIt = std::remove_if(returnList.begin(), returnList.end(), [excludeList](const Object<SysDept>& m) {
            return std::find(excludeList.begin(), excludeList.end(), m) != excludeList.end();
        });
    returnList.erase(delIt, returnList.end());
    return  returnList;
}

bool DeptService::checkDeptNameUnique(const Object<SysDept>& dept) const
{
    //处理一下id
    auto deptId = dept->dept_id.value_or(-1);

    auto info =  m_deptDao->selectByName(*dept->dept_name, *dept->parent_id);
	if (info && info->dept_id != deptId) {
        return false;
    }
    return true;
}

