#pragma once

#include "common/domain/BaseEntity.hpp"

/**
 * 岗位表 post.
 */
struct SysPost : public BaseEntity {
	std::optional<int32_t> post_id;
	std::optional<std::string> post_code;
	std::optional<std::string> post_name;
	std::optional<int32_t> post_sort;
	std::optional<int8_t> is_active;
};

MAYE_JSON_DEFINE_TYPE(SysPost, BASE_ENTITY_JSON_FIELD, post_id, post_code, post_name, post_sort, is_active)
