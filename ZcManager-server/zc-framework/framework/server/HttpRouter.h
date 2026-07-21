#pragma once

#include "hv/HttpServer.h"

class BaseController;

class HttpRouter : public hv::HttpService
{
public:
	void addController(const std::shared_ptr<BaseController> &controller);

private:
	std::vector<std::shared_ptr<BaseController>> m_controllers;
};
