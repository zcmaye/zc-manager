#pragma once

#include "common/base/Types.hpp"
#include <random>

class UserNameService 
{
public:
	enum Type {
		Chinese,
		English,
	};
public:
	UserNameService();
	~UserNameService();

	void setType(Type type);
	Type type()const;

	std::string createUserName();
private:
	Type m_type{ Chinese };

	StringList m_chineseNames;
	std::string createChineseUserName();
	std::string createEnglishUserName();

	std::random_device m_rd;
};

