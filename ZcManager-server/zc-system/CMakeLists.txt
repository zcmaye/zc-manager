#最小cmake版本
cmake_minimum_required (VERSION 3.10)

#项目名称
project(zc-system)

#请求C++标准
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)	#强制请求，如果不支持C++20 则报错 

#添加编译选项
if(MSVC)
	add_compile_options("/utf-8")
endif()

#添加头文件包含目录
include_directories(${CMAKE_INCLUDE_PATH})
#添加库文件包含目录
link_directories(${CMAKE_LIBRARY_PATH})

message("------------" ${CMAKE_LIBRARY_PATH})

# 将源代码添加到此项目的可执行文件。
set(HEADER_FILES
	system/dao/UserDao.h
	system/dao/DeptDao.h
	system/dao/MenuDao.h
	system/dao/PostDao.h
	system/dao/RoleDao.h
	system/dao/RoleMenuDao.h
	system/dao/UserPostDao.h
	system/dao/UserRoleDao.h

	system/service/DeptService.h
	system/service/MenuService.h
	system/service/UserNameService.h
	system/service/PermissionService.h
	system/service/PostService.h
	system/service/RoleService.h
	system/service/UserService.h


)
set(SOURCE_FILES
	system/dao/impl/UserDao.cpp
	system/dao/impl/DeptDao.cpp
	system/dao/impl/MenuDao.cpp
	system/dao/impl/PostDao.cpp
	system/dao/impl/RoleDao.cpp
	system/dao/impl/RoleMenuDao.cpp
	system/dao/impl/UserPostDao.cpp
	system/dao/impl/UserRoleDao.cpp

	system/service/DeptService.cpp
	system/service/MenuService.cpp
	system/service/UserNameService.cpp
	system/service/PermissionService.cpp
	system/service/PostService.cpp
	system/service/RoleService.cpp
	system/service/UserService.cpp
)

#生成静态库
add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

# TODO: 如有需要，请添加测试并安装目标。
target_link_libraries(${PROJECT_NAME} PUBLIC zc-common zc-framework SqlBuilder::SqlBuilder)

#给目标添加头文件包含目录
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

message("---------" ${CMAKE_CURRENT_SOURCE_DIR})