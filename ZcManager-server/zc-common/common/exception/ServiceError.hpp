#pragma once

#include <stdexcept>

/**
 * 服务错误.
 */
class ServiceError : public std::runtime_error
{
public:
	ServiceError(const std::string& msg)
		: runtime_error(msg)
	{
	}
};