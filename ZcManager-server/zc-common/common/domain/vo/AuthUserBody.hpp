#pragma once

#include "common/base/Types.hpp"

/**
 * 岗位选项
 */
struct AuthUserBody{
	int32_t role_id;
	std::vector<int32_t> user_ids;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AuthUserBody, role_id, user_ids)
