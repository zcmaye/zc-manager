#include <iostream>
#include "web/controller/system/LoginController.h"
#include "web/controller/system/RegisterController.hpp"
#include "web/controller/system/UserController.h"
#include "web/controller/system/ProfileController.h"
#include "web/controller/system/DeptController.h"
#include "web/controller/system/PostController.h"
#include "web/controller/system/MenuController.h"
#include "web/controller/system/RoleController.h"

#include "framework/server/HttpRouter.h"
#include "framework/handler/Middleware.h"
#include "framework/config/AppConfig.h"

#include "framework/service/PasswordService.hpp"
#include <csignal>
#include <thread>

void registerHandler(HttpRouter& router)
{
	router.middleware.emplace_back(Middleware::auth);
	router.postprocessor = Middleware::postprocessor;
}

hv::HttpServer server;

void signalHandler(int signum) {
	if (signum == SIGINT || signum == SIGTERM) {
		server.stop();
		std::printf("%d\n", signum);
	}
}

void run()
{
	signal(SIGINT,signalHandler);	//CTRL+C
	signal(SIGTERM,signalHandler);	//kill

	PasswordService pwdService;

	//创建路由
	HttpRouter router;

	//注册处理器
	registerHandler(router);

	//添加控制器
	router.addController(LoginController::createShared());
	router.addController(RegisterController::createShared());
	router.addController(UserController::createShared());
	router.addController(ProfileController::createShared());
	router.addController(DeptController::createShared());
	router.addController(PostController::createShared());
	router.addController(MenuController::createShared());
	router.addController(RoleController::createShared());

	//创建服务器
	server.service = &router;
	server.setPort(AppConfig::instance()->get<int>("port", "app", 8080));

	std::cout << "server start on port:" << server.port << std::endl;

	//运行服务器
	std::thread thread([&] {
		server.run();
		});

	//等待线程结束
	if (thread.joinable()) {
		thread.join();
	}
}

int main()
{
	system("chcp 65001");
	try {
		run();
	}
	catch (const std::exception& e) {
		std::cout << "Exception:" << e.what() << std::endl;
	}
	return 0;
}
