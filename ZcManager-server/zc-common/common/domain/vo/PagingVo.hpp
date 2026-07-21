#pragma once

#include "common/base/Types.hpp"

template<typename T>
struct PagingVo{
	int32_t page;
	int32_t pageSize;
	int32_t totalCount;
	std::vector<std::shared_ptr<T>> items;

	PagingVo()
	: page()
	, pageSize()
	, totalCount()
	{ 
	}
};

template<typename BasicJsonType,typename T, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> 
void to_json(BasicJsonType& nlohmann_json_j, const PagingVo<T>& nlohmann_json_t) {
    nlohmann_json_j["page"] = nlohmann_json_t.page; nlohmann_json_j["pageSize"] = nlohmann_json_t.pageSize; nlohmann_json_j["totalCount"] = nlohmann_json_t.totalCount; nlohmann_json_j["items"] = nlohmann_json_t.items;
} 
template<typename BasicJsonType,typename T, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0>
void from_json(const BasicJsonType& nlohmann_json_j, PagingVo<T>& nlohmann_json_t) {
    const PagingVo<T> nlohmann_json_default_obj{}; 
    if (!nlohmann_json_j.is_null()) {
        if (nlohmann_json_j.contains("page")) {
            auto& v = nlohmann_json_j["page"]; if (v.is_null()) nlohmann_json_t.page = nlohmann_json_default_obj.page; else nlohmann_json_t.page = v;
        }
    } if (!nlohmann_json_j.is_null()) {
        if (nlohmann_json_j.contains("pageSize")) {
            auto& v = nlohmann_json_j["pageSize"]; if (v.is_null()) nlohmann_json_t.pageSize = nlohmann_json_default_obj.pageSize; else nlohmann_json_t.pageSize = v;
        }
    } if (!nlohmann_json_j.is_null()) {
        if (nlohmann_json_j.contains("totalCount")) {
            auto& v = nlohmann_json_j["totalCount"]; if (v.is_null()) nlohmann_json_t.totalCount = nlohmann_json_default_obj.totalCount; else nlohmann_json_t.totalCount = v;
        }
    } if (!nlohmann_json_j.is_null()) {
        if (nlohmann_json_j.contains("items")) {
            auto& v = nlohmann_json_j["items"]; if (v.is_null()) nlohmann_json_t.items = nlohmann_json_default_obj.items; else nlohmann_json_t.items = v;
        }
    }
} 

template<typename BasicJsonType,typename T, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> 
void to_json(BasicJsonType& nlohmann_json_j, const std::shared_ptr<PagingVo<T>>& nlohmann_json_t) {
    nlohmann_json_j["page"] = nlohmann_json_t->page; nlohmann_json_j["pageSize"] = nlohmann_json_t->pageSize; nlohmann_json_j["totalCount"] = nlohmann_json_t->totalCount; nlohmann_json_j["items"] = nlohmann_json_t->items;
}
