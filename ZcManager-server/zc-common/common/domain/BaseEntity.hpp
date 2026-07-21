#pragma once

#include "common/json/njson.hpp"
#include "common/domain/model/QueryParams.hpp"

#include <string>
#include <optional>

struct BaseEntity{
	std::optional<std::string> create_time;
	std::optional<std::string> create_by;
	std::optional<std::string> update_time;
	std::optional<std::string> update_by;
	std::optional<std::string> remark;

	QueryParams params;			/*其他查询参数*/
};

#define BASE_ENTITY_JSON_FIELD create_time,create_by,update_time,update_by,remark
