cmake_minimum_required(VERSION 3.16)

project(zc-admin LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#自动拷贝资源目录
file(COPY ./resource DESTINATION ${PROJECT_BINARY_DIR})

#将需要的库拷贝到二进制目录
if(WIN32)
	file(COPY ${THIRD_PARTY_LIB_FILES} DESTINATION ${PROJECT_BINARY_DIR})
	message(STATUS "copy lib files ${THIRD_PARTY_LIB_FILES} to ${PROJECT_BINARY_DIR}")
endif()

#添加编译时宏
if(USE_REDIS_CACHE)
	add_compile_definitions(USE_REDIS_CACHE)
endif()

# 将源代码添加到此项目的可执行文件。
set(HEADER_FILES
	web/controller/system/LoginController.h
	web/controller/system/UserController.h
	web/controller/system/ProfileController.h
	web/controller/system/DeptController.h
	web/controller/system/PostController.h
	web/controller/system/MenuController.h
	web/controller/system/RoleController.h
)
set(SOURCE_FILES
	App.cpp
	web/controller/system/impl/LoginController.cpp
	web/controller/system/impl/UserController.cpp
	web/controller/system/impl/ProfileController.cpp
	web/controller/system/impl/DeptController.cpp
	web/controller/system/impl/PostController.cpp
	web/controller/system/impl/MenuController.cpp
	web/controller/system/impl/RoleController.cpp
)

add_executable(Zc-Manager-server ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(Zc-Manager-server PUBLIC 
	zc-common 
	zc-system 
	zc-framework
)
