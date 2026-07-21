#pragma once

#include "common/base/Types.hpp"

/**
 * 岗位选项
 */
struct PostOptions {
	int32_t post_id;
	std::string post_name;
	std::string post_code;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PostOptions, post_id, post_name, post_code)
