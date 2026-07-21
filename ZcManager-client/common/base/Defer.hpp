#pragma once

#include <functional>

/**
 * 延迟调用类.
 */
class Defer
{
public:
	Defer(const std::function<void(void)>& fun) 
		:m_call(fun)
	{}
	~Defer() 
	{
		if (m_call) m_call();
	}
private:
	std::function<void(void)> m_call;
};
