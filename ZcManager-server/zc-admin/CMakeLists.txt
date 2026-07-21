cmake_minimum_required (VERSION 3.10)

project ("zc-admin")

#请求C++标准
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)	#强制请求，如果不支持C++20 则报错 
set(CMAKE_INCLUDE_CURRENT_DIR ON)	#自动添加当前目录到include路径

#添加编译选项
if(MSVC)
	add_compile_options("/utf-8")
endif()

#自动拷贝资源目录
file(COPY ./resource DESTINATION ${PROJECT_BINARY_DIR})

#将需要的库拷贝到二进制目录
if(WIN32)
	file(COPY ${THRID_PARTY_LIB_FILES} DESTINATION ${PROJECT_BINARY_DIR})
	message(STATUS "copy lib files ${THRID_PARTY_LIB_FILES} to ${PROJECT_BINARY_DIR}")
endif()

#添加编译时宏
if(USE_REDIS_CACHE)
	add_compile_definitions(USE_REDIS_CACHE)
	#message(STATUS "------------USE_REDIS_CACHE ${USE_REDIS_CACHE}")
endif()


#添加头文件包含目录
include_directories(${CMAKE_INCLUDE_PATH})
#添加库文件包含目录
link_directories(${CMAKE_LIBRARY_PATH})

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

#生成静态库
add_executable(Zc-Manager-server ${HEADER_FILES} ${SOURCE_FILES})

# TODO: 如有需要，请添加测试并安装目标。
target_link_libraries(Zc-Manager-server PUBLIC zc-common zc-system zc-framework)
