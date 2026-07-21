#pragma once

#include <string>
#include <optional>

struct QueryParams {
	int32_t page{ 1 };
	int32_t pageSize{ 10 };
	std::optional<std::string> begTime;
	std::optional<std::string> endTime;

	bool hasBeginTime()const { return begTime.has_value(); }
	bool hasEndTime()const { return begTime.has_value(); }

	int32_t offset()const { return (page - 1) * pageSize; };
	int32_t limit()const { return pageSize; };
};