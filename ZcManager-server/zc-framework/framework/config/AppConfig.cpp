#include "AppConfig.h"
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

AppConfig* AppConfig::instance()
{
	static std::shared_ptr<AppConfig> ins(new AppConfig());
	return ins.get();
}

AppConfig::AppConfig()
{
	if (m_iniParser.LoadFromFile("./resource/application.ini") != 0) {
		std::printf("load ini file failed!\n");
		return;
	}

	//检查目录是否存在
	std::error_code errc;
	fs::create_directories(getProfile(), errc);
	if (errc) {
		std::printf("create profile directory failed!\n");
	}

	fs::create_directories(getAvatarPath(),errc);
	if (errc) {
		std::printf("create avatar directory failed!\n");
	}
}

std::string AppConfig::getProfile() const
{
#ifdef _WIN32
	return m_iniParser.GetValue("win_profile", "app");
#else
	return m_iniParser.GetValue("linux_profile", "app");
#endif
}

std::string AppConfig::getAvatarPath() const
{
	return getProfile() + "/avatar";
}
