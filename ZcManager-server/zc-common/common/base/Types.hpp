#pragma once

#include "nlohmann/json.hpp"

#include <iostream>
#include <memory>
#include <vector>
#include <format>
#include <string>
#include <set>
#include <optional>

template<typename T>
using Object = std::shared_ptr<T>;

template<typename T>
using ObjectList = std::vector<Object<T>>;

using StringList = std::vector<std::string>;


/**
 * 序列化ObjectList类型.
 */
template<typename BasicJsonType,typename T, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> 
void to_json(BasicJsonType& nlohmann_json_j, const ObjectList<T>& nlohmann_json_t) {
    if (nlohmann_json_t.empty()) {
        return;
    }

    for (auto& v : nlohmann_json_t) {
        nlohmann::json j = *v;
        nlohmann_json_j.push_back(j);
    }
} 
/**
 * 反序列化ObjectList类型.
 */
template<typename BasicJsonType,typename T, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> 
void from_json(const BasicJsonType& nlohmann_json_j, ObjectList<T>& nlohmann_json_t) {
    if (nlohmann_json_j.is_null()) {
        return;
    }

    for (auto& j : nlohmann_json_j) {
        T v = j;
        nlohmann_json_t.push_back(std::make_shared<T>(v));
    }
}

/*
* Object的序列化
*/
template<typename BasicJsonType,typename T, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> 
void to_json(BasicJsonType& nlohmann_json_j, const Object<T>& nlohmann_json_t) {
    if (!nlohmann_json_t) {
		std::cout << typeid(T).name() << " Object is null" << std::endl;
        return;
    }
    nlohmann_json_j = *nlohmann_json_t;
};
