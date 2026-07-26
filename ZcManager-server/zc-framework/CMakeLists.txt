cmake_minimum_required(VERSION 3.16)

project(zc-framework LANGUAGES CXX)

message(STATUS "=======CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

#查找包
find_package(libhv REQUIRED)

# 将源代码添加到此项目的可执行文件。
set(HEADER_FILES
	framework/server/HttpRouter.h
	framework/handler/Middleware.h
	framework/config/AppConfig.h
	framework/context/ContextHolder.h
)
set(SOURCE_FILES
	framework/server/HttpRouter.cpp
	framework/handler/Middleware.cpp
	framework/config/AppConfig.cpp
	framework/context/ContextHolder.cpp
)

#生成静态库
add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC 
	hv 
	zc-common
)

#给目标添加头文件包含目录
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})