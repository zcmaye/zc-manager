#include "../ProfileController.h"

#include "system/service/UserService.h"
#include "framework/config/AppConfig.h"
#include "framework/service/TokenService.hpp"
#include "common/utils/DateTime.hpp"
#include "common/utils/EncryptUtils.hpp"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class ProfileControllerImpl : public ProfileController
{
	UserService m_userService;
public:
	void registerRouters(hv::HttpService* router) override
	{
		ENDPOINT(router, POST, "/profile/avatar", avatar);
		ENDPOINT(router, GET, "/profile/avatar", getAvatar);
		ENDPOINT(router, PUT, "/profile", updateProfile);
		ENDPOINT(router, PUT, "/profile/updatePwd", updatePwd);
	}

	int avatar(const HttpContextPtr& ctx) override
	{
		//获取上传的文件数据
		const auto& parts = ctx->form();
		//是否有数据
		if (parts.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("请上传一张图片"));
		}
		//获取第一个part
		auto& part = parts.begin()->second;
		//保存文件
		//-- 获取头像保存路径
		auto avatar_file = getSaveAvatarPath(getAvatarFilename(part.filename));
		std::fstream file(avatar_file, std::ios::out | std::ios::binary);
		if (!file) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("头像保存失败，请联系管理员~"));
		}
		file.write(part.content.data(), part.content.size());

		//将头像保存到数据库中
		m_userService.updateAvatar(getUserId(), getAvatarPath(avatar_file));
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("头像上传成功~"));
	}

	int getAvatar(const HttpContextPtr& ctx) override
	{
		auto& user = getLoginUser()->user;
		if (!user) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("用户信息不存在~"));
		}
		if (!user->avatar) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("用户没有头像~"));
		}
		if (user->avatar->empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("用户没有头像~"));
		}
		auto avatar = getRealAvatarPath(*user->avatar);
		if (!fs::exists(avatar)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("用户头像丢失~"));
		}

		ctx->sendFile(avatar.data());
		return HTTP_STATUS_OK;
	}

	int updateProfile(const HttpContextPtr& ctx) override
	{
		auto user = std::make_shared<SysUser>();
		*user = nlohmann::json::parse(ctx->body());

		auto loginUser =  getLoginUser();
		auto& currentUser = loginUser->user;
		currentUser->nick_name = user->nick_name;
		currentUser->email = user->email;
		currentUser->phone_number = user->phone_number;
		currentUser->sex = user->sex;
		currentUser->remark = user->remark;


		//检查唯一性
		if (user->email && !m_userService.checkEmailUnique(user)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改资料'" + *currentUser->user_name + "'失败，邮箱已存在~"));
		}
		else if (user->phone_number && !m_userService.checkPhoneNumberUnique(user)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改资料'" + *currentUser->user_name + "'失败，手机号已存在~"));
		}

		if (m_userService.updateUser(user)) {
			TokenService().setLoginUser(loginUser);
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("修改个人信息成功~"));
		}
		return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改个人信息失败~"));
	}

	int updatePwd(const HttpContextPtr& ctx) override
	{
		auto j = nlohmann::json::parse(ctx->body());

		auto oldPwd = j.value("oldPassword","");
		auto newPwd = j.value("newPassword", "");

		if (oldPwd.empty() || newPwd.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("用户密码修改失败!密码为空~"));
		}

		oldPwd = EncryptUtils::encrpytPassword(oldPwd);
		newPwd = EncryptUtils::encrpytPassword(newPwd);

		auto loginUser = getLoginUser();
		auto userName = loginUser->user->user_name;
		auto pwd = loginUser->user->password;

		if(oldPwd != *pwd){
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("修改密码失败，旧密码错误~"));
		}
		else if (newPwd == *pwd) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("新密码不能和旧密码相同~"));
		}

		//执行更新
		auto result = Result::success();
		auto newUser = std::make_shared<SysUser>();
		newUser->user_id = loginUser->user->user_id;
		newUser->password = newPwd;
		newUser->update_by = loginUser->user->user_name;

		if (!m_userService.updatePwd(newUser)) {
			result.setMessage("用户密码更新失败~");
		}
		else {
			result.setMessage("用户密码更新成功~");
		}
		return sendResult(ctx, HTTP_STATUS_OK, result);
	}

public:
	std::string getAvatarFilename(const std::string& filename)
	{
		std::string extension;
		fs::path path(filename);
		//有拓展名
		if (path.has_extension()) {
			extension = path.extension().generic_string();
		}
		return getUsername() + extension;
	}

	/**
	 * 获取保存到磁盘中的头像路径.
	 */
	std::string getSaveAvatarPath(const std::string& path) {
		//获取磁盘路径
		auto avatar_path = AppConfig::instance()->getAvatarPath();
		//获取日期路径
		auto dtStr = Date::currentDate().toString();
		fs::create_directories(avatar_path + "/" + dtStr);
		//拼接文件路径
		return avatar_path + "/" + dtStr + +"/" + path;
	}


	/**
	 * 获取保存到数据库中的头像路径.
	 */
	std::string getAvatarPath(const std::string& path) {
		auto pos = path.find("/profile");
		if (pos == std::string::npos) {
			return path;
		}
		return path.substr(pos);
	}

	/**
	 * 获取头像在磁盘上的路径.
	 */
	std::string getRealAvatarPath(const std::string& path) {
		auto pos = path.find("/profile");
		if (pos == std::string::npos) {
			return path;
		}
		return AppConfig::instance()->getProfile() + path.substr(pos + strlen("/profile"));
	}

};

std::shared_ptr<ProfileController> ProfileController::createShared()
{
	return std::make_shared<ProfileControllerImpl>();
}
