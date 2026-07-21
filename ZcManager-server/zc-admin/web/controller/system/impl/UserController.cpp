#include "../UserController.h"

#include "system/service/UserService.h"
#include "system/service/PostService.h"
#include "system/service/RoleService.h"
#include "common/constants/UserConstants.hpp"
#include "common/utils/StringUtils.h"
#include "web/utils/QueryParamsMapping.hpp"

class UserControllerImpl : public UserController
{
	UserService m_userService;
	PostService m_postService;
	RoleService m_roleService;
public:
	void registerRouters(hv::HttpService* router) override
	{
		ENDPOINT(router, GET, "/user/list", list);
		ENDPOINT(router, POST, "/user/add", add);
		ENDPOINT(router, PUT, "/user/edit", edit);
		ENDPOINT(router, PUT, "/user/updateStatus", updateStatus);
		ENDPOINT(router, PUT, "/user/resetPwd", resetPwd);
		ENDPOINT(router, Delete, "/user/{userIds}", remove);
		ENDPOINT(router, POST, "/user/import", import_);
		ENDPOINT(router, GET, "/user/export", export_);

		//这个端点必须放到最后，否则会覆盖掉/user/list
		ENDPOINT(router, GET, "/user/{userId}", getInfo);
	}
	int list(const HttpContextPtr& ctx) override
	{
		auto user = QueryParamsMapping::mapTo<SysUser>(ctx->params());

		//调用服务
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_userService.selectUserList(user, user->params.page, user->params.pageSize)));
	}

	int getInfo(const HttpContextPtr& ctx) override
	{
		//检查参数
		std::optional<int32_t> userId;
		if (auto v = ctx->param("userId"); !v.empty()) {
			userId = std::stoi(v);
		}

		zc::mysql::Transaction trans(DEFAULT_CONNECTION_POOL());

		auto result = Result::success();
		if (userId && userId != 0) {
			//调用服务,查询用户信息
			auto user = m_userService.selectUserByUserId(*userId, trans.getConnection());
			result.setData(user);

			//查询岗位
			auto postIds = m_postService.selectPostByUserId(*userId, trans.getConnection());
			result.put("postIds", postIds);
			result.put("roleIds", user->roleIds);
		}

		//查询所有角色
		auto roles = m_roleService.selectRoleAll(trans.getConnection());
		//获取不是管理员的角色
		ObjectList<SysRole>  noAdminRoles;
		std::copy_if(roles.begin(), roles.end(), std::back_inserter(noAdminRoles), [](const auto& role) {
				return !role->isAdmin();
			});
		result.put("roles", SysUser::isAdmin(userId ? *userId : 0) ? roles : noAdminRoles);
		result.put("posts", m_postService.selectPostAll(trans.getConnection()));

		return sendResult(ctx, HTTP_STATUS_OK, result);
	}

	int add(const HttpContextPtr& ctx) override
	{
		auto user = std::make_shared<SysUser>();
		*user = hv::Json::parse(ctx->body());

		//检查参数
		if (!user->user_name) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增用户失败，缺少用户名~"));
		}
		else if (!user->nick_name) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增用户失败，缺少用户昵称~"));
		}
		else if (!user->password) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增用户失败，缺少用户密码~"));
		}

		//检查唯一性
		if (!m_userService.checkUserNameUnique(user)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增用户'" + *user->user_name + "'失败，用户名已存在~"));
		}
		else if (user->email && !m_userService.checkEmailUnique(user)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增用户'" + *user->user_name + "'失败，邮箱已存在~"));
		}
		else if (user->phone_number && !m_userService.checkPhoneNumberUnique(user)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新增用户'" + *user->user_name + "'失败，手机号已存在~"));
		}

		//创建人
		user->create_by = getUsername();

		//执行插入
		m_userService.insertUser(user);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("用户新增成功~"));
	}

	int edit(const HttpContextPtr& ctx) override
	{
		auto user = std::make_shared<SysUser>();
		*user = hv::Json::parse(ctx->body());

		//检查参数
		if (!user->user_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改用户失败，缺少用户ID~"));
		}

		//检查唯一性
		if (user->email && !m_userService.checkEmailUnique(user)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改用户'" + *user->user_name + "'失败，邮箱已存在~"));
		}
		else if (user->phone_number && !m_userService.checkPhoneNumberUnique(user)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改用户'" + *user->user_name + "'失败，手机号已存在~"));
		}

		//创建人
		user->update_by = getUsername();

		//执行插入
		m_userService.updateUser(user);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("用户更新成功~"));
	}

	int updateStatus(const HttpContextPtr& ctx) override
	{
		auto user = std::make_shared<SysUser>();
		*user = hv::Json::parse(ctx->body());

		//检查参数
		if (!user->user_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("更新用户状态失败，缺少用户ID~"));
		}
		else if (!user->status) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("更新用户状态失败，缺少用户状态~"));
		}

		//检查权限
		if (SysUser::isAdmin(user->user_id)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("更新用户状态失败，不允许禁用管理员~"));
		}

		//执行更新
		auto result = Result::success();
		if (!m_userService.updateUserStatus(user)) {
			result.setMessage("用户状态更新失败~");
			return sendResult(ctx, HTTP_STATUS_OK, result);
		}

		if (user->status == UserConstants::DISABLED) {
			result.setMessage("账户已禁用，用户将无法登录~");
		}
		else {
			result.setMessage("账户已启用~");
		}
		return sendResult(ctx, HTTP_STATUS_OK, result);
	}

	int resetPwd(const HttpContextPtr& ctx) override
	{
		auto user = std::make_shared<SysUser>();
		*user = hv::Json::parse(ctx->body());

		//检查参数
		if (!user->user_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("用户密码修改失败，缺少用户ID~"));
		}
		else if (!user->password) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("用户密码修改失败，缺少密码~"));
		}

		user->update_by = getUsername();

		//执行更新
		auto result = Result::success();
		if (!m_userService.updatePwd(user)) {
			result.setMessage("用户密码更新失败~");
			return sendResult(ctx, HTTP_STATUS_OK, result);
		}
		else {
			result.setMessage("用户密码更新成功~");
		}
		return sendResult(ctx, HTTP_STATUS_OK, result);
	}

	int remove(const HttpContextPtr& ctx) override
	{
		auto userIds_str = ctx->param("userIds");
		if (userIds_str.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("删除失败，缺少用户ID~"));
		}

		auto userIds = StringUtils::split<int>(userIds_str, ',');
		if (userIds.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("删除失败，缺少用户ID~"));
		}

		//删除
		m_userService.deleteUsers(userIds);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("删除成功~"));
	}

	int import_(const HttpContextPtr& ctx) override
	{
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("/user/import"));
	}

	int export_(const HttpContextPtr& ctx) override
	{
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("/user/export"));
	}
};

std::shared_ptr<UserController> UserController::createShared()
{
	return std::make_shared<UserControllerImpl>();
}
