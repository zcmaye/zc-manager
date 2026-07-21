cmake_minimum_required (VERSION 3.10)

project ("Zc-Manager-server")

option(USE_REDIS_CACHE "Enable Redis Cache" OFF)

if(WIN32)
	# 设置三方库目录
	set(THRID_PARTY_DIR ${CMAKE_SOURCE_DIR}/third-party)
	#message(STATUS "===THRID_PARTY_DIR: ${THRID_PARTY_DIR}")
	# 递归搜索当前目录下所有dll文件
	file(GLOB_RECURSE THRID_PARTY_LIB_FILES "${THRID_PARTY_DIR}/*.dll")
	#message(STATUS "===THRID_PARTY_LIB_FILES: ${THRID_PARTY_LIB_FILES}")
endif()

#添加子目录(子目录中的必须能生成库文件)
add_subdirectory(zc-admin)

add_subdirectory(zc-common)

add_subdirectory(zc-system)

add_subdirectory(zc-framework)

add_subdirectory(third-party/SqlBuilder)

add_subdirectory(third-party/json-3.12.0)

