#pragma once

#include "common/base/Types.hpp"

struct SysUserPost{
	int32_t user_id;
	int32_t post_id;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SysUserPost, user_id, post_id)
