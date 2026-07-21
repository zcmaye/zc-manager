cmake_minimum_required(VERSION 3.16)

project(zc-common LANGUAGES CXX)

#查找OpenSSL
find_package(OpenSSL REQUIRED)

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

#基础依赖
target_link_libraries(${PROJECT_NAME} PUBLIC 
	mysqlcppconn 
	nlohmann_json::nlohmann_json
	OpenSSL::SSL 
	OpenSSL::Crypto
)

#Redis 依赖（条件编译）
if(USE_REDIS_CACHE)
	find_package(hiredis REQUIRED)
	find_package(redis++ REQUIRED)
	target_link_libraries(${PROJECT_NAME} PUBLIC 
		hiredis::hiredis
		redis++::redis++
	)
endif()

#给目标添加头文件包含目录
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})