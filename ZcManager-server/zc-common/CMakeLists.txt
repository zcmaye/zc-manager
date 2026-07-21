#最小cmake版本
cmake_minimum_required (VERSION 3.10)

#项目名称
project(zc-common)

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

find_package(hiredis REQUIRED)
find_package(redis++ REQUIRED)
find_package(OpenSSL REQUIRED)

message("------------" ${CMAKE_LIBRARY_PATH})

# 将源代码添加到此项目的可执行文件。
set(HEADER_FILES
	common/sql/ConnectionPool.h
	common/utils/StringUtils.h
)
set(SOURCE_FILES
	common/sql/ConnectionPool.cpp
	common/utils/StringUtils.cpp
)

#生成静态库
add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

# TODO: 如有需要，请添加测试并安装目标。
target_link_libraries(${PROJECT_NAME} PUBLIC 
mysqlcppconn 
nlohmann_json::nlohmann_json
hiredis::hiredis
redis++::redis++
OpenSSL::SSL OpenSSL::Crypto
)

#给目标添加头文件包含目录
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})