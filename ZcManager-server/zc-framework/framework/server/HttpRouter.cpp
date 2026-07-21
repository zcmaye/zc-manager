#include "HttpRouter.h"
#include "../controller/BaseController.hpp"

void HttpRouter::addController(const std::shared_ptr<BaseController>& controller)
{
	if (!controller) {
		return;
	}
	controller->registerRouters(this);
	m_controllers.push_back(controller);
}
