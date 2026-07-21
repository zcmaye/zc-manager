#pragma once

#include "common/base/Types.hpp"

struct SysRoleMenu{
	int32_t role_id;
	int32_t menu_id;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SysRoleMenu, role_id, menu_id)
