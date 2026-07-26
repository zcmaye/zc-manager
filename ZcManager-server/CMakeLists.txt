cmake_minimum_required(VERSION 3.16)

project(Zc-Manager-server LANGUAGES CXX)

#C++标准配置
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#生成编译数据库（用于IDE代码补全）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#编译选项配置
if(MSVC)
	add_compile_options("/utf-8")
	add_compile_options("/W4")
endif()

#选项配置
option(USE_REDIS_CACHE "Enable Redis Cache" OFF)

#第三方库目录
if(WIN32)
	set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third-party)
	file(GLOB_RECURSE THIRD_PARTY_LIB_FILES "${THIRD_PARTY_DIR}/*.dll")
	
	#添加第三方库的头文件包含路径
	include_directories(
		${THIRD_PARTY_DIR}/mysql-connector-c++-9.2.0-winx64/include
	)
	
	#添加第三方库的库文件搜索路径
	link_directories(
		${THIRD_PARTY_DIR}/mysql-connector-c++-9.2.0-winx64/lib64/debug/vs14
	)
endif()

#添加子目录
add_subdirectory(third-party/SqlBuilder)

add_subdirectory(third-party/json-3.12.0)

add_subdirectory(zc-common)
add_subdirectory(zc-framework)
add_subdirectory(zc-system)
add_subdirectory(zc-admin)

