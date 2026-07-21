#pragma once

#include <memory>
#include <iostream>
#include <mutex>

template <typename T>
class Singleton
{
public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(Singleton&) = delete;

	Singleton() {}
	virtual ~Singleton() {
		std::cout << "destroyed " << typeid(T).name() << std::endl;
	}

	static std::shared_ptr<T> instance() {
		static std::shared_ptr<T> ptr;
		static std::once_flag flag;
		std::call_once(flag, [&] {
				ptr.reset(new T);
			});
		return ptr;
	}

	void printAddress()const
	{
		std::cout << "address is :" << instance().get() << std::endl;
	}
};
