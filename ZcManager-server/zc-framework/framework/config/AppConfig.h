#pragma once

#include "hv/iniparser.h"

class AppConfig{
public:
	static AppConfig* instance();
	AppConfig();

	template<typename T>
	inline T get(const char* key, const char* section, const T& def = T{});


	std::string getProfile()const;
	std::string getAvatarPath()const;
private:
	mutable IniParser m_iniParser;
};

template<>
inline std::string AppConfig::get<std::string>(const char* key, const char* section, const std::string& def)
{
	return m_iniParser.GetValue(key, section);
}

template<>
inline bool AppConfig::get<bool>(const char* key, const char* section,const bool& def) {
	return m_iniParser.Get<bool>(key, section);
}

template<>
inline int AppConfig::get<int>(const char* key, const char* section,const int& def) {
	return m_iniParser.Get<int>(key, section);
}

template<>
inline int16_t AppConfig::get<int16_t>(const char* key, const char* section,const int16_t& def) {
	return static_cast<int16_t>(m_iniParser.Get<int>(key, section));
}

template<>
inline float AppConfig::get<float>(const char* key, const char* section,const float& def) {
	return m_iniParser.Get<float>(key, section);
}


