#pragma once

#include "common/base/Types.hpp"

struct SysUserRole{
	int32_t user_id;
	int32_t role_id;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT (SysUserRole, user_id, role_id)
