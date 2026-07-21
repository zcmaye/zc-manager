#pragma once

#include <stdexcept>

/**
 * 业务错误.
 */
class BusinessError : public std::runtime_error
{
public:
	BusinessError(const std::string& msg)
		: runtime_error(msg)
	{
	}
};