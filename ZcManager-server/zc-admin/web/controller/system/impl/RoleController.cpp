#include "../RoleController.h"


#include "system/service/RoleService.h"
#include "system/service/UserService.h"
#include "system/domain/entity/SysUserRole.hpp"
#include "common/constants/UserConstants.hpp"
#include "common/domain/vo/AuthUserBody.hpp"
#include "common/utils/StringUtils.h"

#include "web/utils/QueryParamsMapping.hpp"

class RoleControllerImpl : public RoleController
{
	RoleService m_roleService;
	UserService m_userService;
public:
	void registerRouters(hv::HttpService* router) override
	{
		ENDPOINT(router, GET, "/role/list", list);
		ENDPOINT(router, GET, "/role/options", options);
		ENDPOINT(router, POST, "/role/add", add);
		ENDPOINT(router, PUT, "/role/edit", edit);
		ENDPOINT(router, PUT, "/role/updateStatus", updateStatus);
		ENDPOINT(router, Delete, "/role/{roleIds}", remove);
		ENDPOINT(router, GET, "/role/authUser/allocatedList", allocatedList);
		ENDPOINT(router, GET, "/role/authUser/unallocatedList", unallocatedList);
		ENDPOINT(router, PUT, "/role/authUser/cancel", cancelAuthUser);
		ENDPOINT(router, PUT, "/role/authUser/cancelAll", cancelAuthUserAll);
		ENDPOINT(router, PUT, "/role/authUser/selectAll", selectAuthUserAll);

		//这个端点必须放到最后，否则会覆盖掉/role/list
		ENDPOINT(router, GET, "/role/{roleId}", getInfo);
	}
	int list(const HttpContextPtr& ctx) override
	{
		auto role = QueryParamsMapping::mapTo<SysRole>(ctx->params());

		//调用服务
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_roleService.selectRoleList(role)));
	}

	int getInfo(const HttpContextPtr& ctx) override
	{
		//检查参数
		int32_t roleId;
		if (auto v = ctx->param("roleId"); v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少角色ID"));
		}
		else {
			roleId = std::stoi(v);
			if (roleId <= 0) {
				return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少角色ID"));
			}
		}
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_roleService.selectRoleById(roleId)));
	}

	int add(const HttpContextPtr& ctx) override
	{
		auto role = std::make_shared<SysRole>();
		*role = hv::Json::parse(ctx->body());

		//检查参数
		if (!role->role_name) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增角色失败，缺少角色名~"));
		}
		else if (!role->role_key) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增角色失败，缺少角色key~"));
		}
		else if (!role->role_sort) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增角色失败，缺少角色排序~"));
		}

		//检查唯一性
		if (!m_roleService.checkRoleKeyUnique(role)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增角色'" + *role->role_name + "'失败，角色key已存在~"));
		}
		else if (!m_roleService.checkRoleNameUnique(role)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增角色'" + *role->role_name + "'失败，角色名称已存在~"));
		}

		//创建人
		role->create_by = getUsername();

		//执行插入
		m_roleService.insertRole(role);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("角色新增成功~"));
	}

	int edit(const HttpContextPtr& ctx) override
	{
		auto role = std::make_shared<SysRole>();
		*role = hv::Json::parse(ctx->body());

		//检查参数
		if (!role->role_name) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改角色失败，缺少角色名~"));
		}
		else if (!role->role_key) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改角色失败，缺少角色key~"));
		}

		//检查唯一性
		if (!m_roleService.checkRoleKeyUnique(role)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改角色'" + *role->role_name + "'失败，角色key已存在~"));
		}
		else if (!m_roleService.checkRoleNameUnique(role)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改角色'" + *role->role_name + "'失败，角色名称已存在~"));
		}

		//创建人
		role->update_by = getUsername();

		m_roleService.checkRoleAllowed(*role->role_id);
		//执行插入
		m_roleService.updateRole(role);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("角色更新成功~"));
	}

	int updateStatus(const HttpContextPtr& ctx) override
	{
		auto role = std::make_shared<SysRole>();
		*role = hv::Json::parse(ctx->body());

		//检查参数
		if (!role->role_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("更新角色状态失败，缺少角色ID~"));
		}
		else if (!role->is_active) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("更新角色状态失败，缺少角色状态~"));
		}

		m_roleService.checkRoleAllowed(*role->role_id);
		//执行更新
		auto result = Result::success();
		if (!m_roleService.updateRoleStatus(role)) {
			result.setMessage("角色状态更新失败~");
			return sendResult(ctx, HTTP_STATUS_OK, result);
		}

		if (role->is_active == UserConstants::DISABLED) {
			result.setMessage("角色已禁用~");
		}
		else {
			result.setMessage("角色已启用~");
		}
		return sendResult(ctx, HTTP_STATUS_OK, result);
	}

	int remove(const HttpContextPtr& ctx) override
	{
		auto ids_str = ctx->param("roleIds");
		if (ids_str.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("删除失败，缺少角色ID~"));
		}

		auto ids = StringUtils::split<int>(ids_str, ',');
		if (ids.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("删除失败，缺少角色ID~"));
		}

		//删除
		m_roleService.deleteRoles(ids);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("删除成功~"));
	}

	int options(const HttpContextPtr& ctx) {
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_roleService.selectRoleAll()));
	}
public:
	int allocatedList(const HttpContextPtr& ctx) override
	{
		auto user = std::make_shared<SysUser>();
		if (auto v = ctx->param("role_id"); v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少角色ID"));
		}
		else {
			user->role_id = std::stoi(v);
		}

		if (auto v = ctx->param("user_name"); !v.empty()) {
			user->user_name = v;
		}
		if (auto v = ctx->param("phone_number"); !v.empty()) {
			user->phone_number = v;
		}
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_userService.selectAllocatedList(user)));
	}
	int unallocatedList(const HttpContextPtr& ctx) override
	{
		auto user = std::make_shared<SysUser>();
		if (auto v = ctx->param("role_id"); v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少角色ID"));
		}
		else{
			user->role_id = std::stoi(v);
		}
		if (auto v = ctx->param("user_name"); !v.empty()) {
			user->user_name = v;
		}
		if (auto v = ctx->param("phone_number"); !v.empty()) {
			user->phone_number = v;
		}
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_userService.selectUnallocatedList(user)));
	}
	int cancelAuthUser(const HttpContextPtr& ctx) override
	{
		auto ur = std::make_shared<SysUserRole>();
		*ur = hv::Json::parse(ctx->body());

		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_roleService.deletedAuthUser(ur)));
	}
	int cancelAuthUserAll(const HttpContextPtr& ctx) override
	{
		auto ur = std::make_shared<AuthUserBody>();
		*ur = hv::Json::parse(ctx->body());
		m_roleService.checkRoleAllowed(ur->role_id);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_roleService.deletedAuthUsers(ur->role_id, ur->user_ids)));
	}
	int selectAuthUserAll(const HttpContextPtr& ctx) override
	{
		auto ur = std::make_shared<AuthUserBody>();
		*ur = hv::Json::parse(ctx->body());
		m_roleService.checkRoleAllowed(ur->role_id);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_roleService.insertAuthUsers(ur->role_id, ur->user_ids)));
	}
};

std::shared_ptr<RoleController> RoleController::createShared()
{
	return std::make_shared<RoleControllerImpl>();
}

