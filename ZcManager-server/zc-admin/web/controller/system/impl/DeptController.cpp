#include "../DeptController.h"

#include "system/service/DeptService.h"
#include "common/constants/UserConstants.hpp"
#include "web/utils/QueryParamsMapping.hpp"

class DeptControllerImpl : public DeptController
{
	DeptService m_deptService;
public:
	void registerRouters(hv::HttpService* router) override
	{
		ENDPOINT(router, GET, "/dept/tree", tree);
		ENDPOINT(router, GET, "/dept/options", options);
		ENDPOINT(router, POST, "/dept/add", add);
		ENDPOINT(router, PUT, "/dept/edit", edit);
		ENDPOINT(router, Delete, "/dept/{deptId}", remove);

		//这个端点必须放到最后，否则会覆盖掉/dept/list
		ENDPOINT(router, GET, "/dept/{deptId}", getInfo);
	}
	int tree(const HttpContextPtr& ctx) override
	{
		//获取参数
		auto dept = QueryParamsMapping::mapTo<SysDept>(ctx->params());

		//调用服务
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_deptService.getDeptTree(dept)));
	}

	int options(const HttpContextPtr& ctx) override
	{
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_deptService.getDeptTreeSelect(std::make_shared<SysDept>())));
	}

	int getInfo(const HttpContextPtr& ctx) override
	{
		//检查参数
		int32_t deptId;
		if (auto v = ctx->param("deptId"); v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少部门ID"));
		}
		else {
			deptId = std::stoi(v);
			if (deptId <= 0) {
				return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少部门ID"));
			}
		}

		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_deptService.selectDeptById(deptId)));
	}

	int add(const HttpContextPtr& ctx) override
	{
		auto dept = std::make_shared<SysDept>();
		*dept = hv::Json::parse(ctx->body());

		//检查参数
		if (!dept->dept_name) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增部门失败，缺少部门名~"));
		}
		else if (!dept->parent_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增部门失败，缺少父部门"));
		}

		//检查唯一性
		if (!m_deptService.checkDeptNameUnique(dept)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增部门'" + *dept->dept_name + "'失败，部门名已存在~"));
		}

		//创建人
		dept->create_by = getUsername();

		//执行插入
		m_deptService.insertDept(dept);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("部门新增成功~"));
	}

	int edit(const HttpContextPtr& ctx) override
	{
		auto dept = std::make_shared<SysDept>();
		*dept = hv::Json::parse(ctx->body());

		//检查参数
		if (!dept->dept_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改部门失败，缺少部门ID~"));
		}

		//检查唯一性
		if (!dept->parent_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改部门失败，父部门缺失~"));
		}
		else if (dept->dept_name && !m_deptService.checkDeptNameUnique(dept)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改部门'" + *dept->dept_name + "'失败，邮箱已存在~"));
		}
		else if (dept->dept_id == dept->parent_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改部门'" + *dept->dept_name + "'失败，上级部门不能是自己~"));
		}
		else if (dept->is_active && dept->is_active == UserConstants::DISABLED&&
			m_deptService.selectNormalChildrenByDeptId(*dept->dept_id) > 0) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("该部门包含未停用的子部门！"));
		}

		//创建人
		dept->update_by = getUsername();

		//执行插入
		m_deptService.updateDept(dept);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("部门更新成功~"));
	}

	int remove(const HttpContextPtr& ctx) override
	{
		//获取要删除的部门ID
		int32_t deptId;
		if (auto v = ctx->param("deptId"); v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("删除失败，缺少部门ID~"));
		}
		else {
			deptId = std::stoi(v);
		}

		if (m_deptService.hasChildByDeptId(deptId)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("存在下级部门，不允许删除~"));
		}

		if (m_deptService.checkDeptExistUser(deptId)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("部门存在用户，不允许删除~"));
		}

		//删除
		m_deptService.deleteDeptById({deptId});
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("删除成功~"));
	}

};

std::shared_ptr<DeptController> DeptController::createShared()
{
	return std::make_shared<DeptControllerImpl>();
}
