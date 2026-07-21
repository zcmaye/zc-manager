#pragma once

#include "nlohmann/json.hpp"
#include "common/base/Types.hpp"
#include "common/base/TypeTraits.h"

using njson_t = nlohmann::json;

/******************generanal*******************/
template<typename T,std::enable_if_t<!zc::is_base_type_v<T>, int> = 0>
void json_to_value(const nlohmann::json& j, T& v)
{
    if (j.is_null()) {
		v = T{};
    }
    else {
        v = j;
    }
}

template<typename T,std::enable_if_t<!zc::is_base_type_v<T>, int> = 0>
void json_from_value(nlohmann::json& j,const T& v)
{
	j = v;
}


/******************base_type*******************/
template<typename T,std::enable_if_t<zc::is_base_type_v<T>, int> = 0>
void json_to_value(const nlohmann::json& j, T& v)
{
    if (j.is_null()) {
		v = T{};
    }
    else {
        v = j;
    }
}

template<typename T,std::enable_if_t<zc::is_base_type_v<T>, int> = 0>
void json_from_value(nlohmann::json& j,const T& v)
{
	j = v;
}


/******************optional*******************/
template<typename T>
void json_to_value(const nlohmann::json& j, std::optional<T>& v)
{
    if (j.is_null()) {
        v = std::nullopt;
    }
    else {
        v = j;
    }
}

template<typename T>
void json_from_value(nlohmann::json& j,const std::optional<T>& v)
{
    if (!v.has_value()) {
        j = nullptr;
    }
    else {
        j = v.value();
    }
}

/******************ObjectList*******************/
template<typename T>
void json_to_value(const nlohmann::json& j, ObjectList<T>& v)
{
    if (!j.is_null()) {
        v = j;
    }
}

template<typename T>
void json_from_value(nlohmann::json& j,const ObjectList<T>& v)
{
    if (!v.empty()) {
        j = v;
    }
}

template<typename T>
void json_to_value(const nlohmann::json& j, Object<T>& v)
{
    if (!j.is_null()) {
        v = std::make_shared<T>();
        *v = j;
    }
}

template<typename T>
void json_from_value(nlohmann::json& j,const Object<T>& v)
{
    if (v) {
        j = *v;
    }
}


/******************std::vector<T>*******************/
template<typename T>
void json_to_value(const nlohmann::json& j, std::vector<T>& v)
{
    if (!j.is_null()) {
        for (auto& item : j) {
            v.push_back(item);
        }
    }
}

template<typename T>
void json_from_value(nlohmann::json& j,const std::vector<T>& v)
{
    if (!v.empty()) {
        for (auto& item : v) {
            j.push_back(item);
        }
    }
}

/******************std::set*******************/
template<typename T>
void json_to_value(const nlohmann::json& j, std::set<T>& v)
{
    if (!j.is_null()) {
        for (auto& item : j) {
			v.insert((const T&)item);
        }
    }
}

template<typename T>
void json_from_value(nlohmann::json& j,const std::set<T>& v)
{
	j = v;
}



/*
* 自定义JSON序列化和反序列化宏
*/
#define MAYE_NLOHMANN_JSON_TO(v1) json_from_value(nlohmann_json_j[#v1], nlohmann_json_t.v1);

#define MAYE_NLOHMANN_JSON_FROM_WITH_DEFAULT(v1)\
		if (!nlohmann_json_j.is_null()) {\
			if (nlohmann_json_j.contains(#v1)) {\
				auto& v = nlohmann_json_j[#v1];\
				if (v.is_null())\
					nlohmann_json_t.v1 = nlohmann_json_default_obj.v1;\
				else\
					json_to_value(v, nlohmann_json_t.v1);\
			}\
		}

#define MAYE_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Type, ...)  \
    template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> \
    void to_json(BasicJsonType& nlohmann_json_j, const Type& nlohmann_json_t) { NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(MAYE_NLOHMANN_JSON_TO, __VA_ARGS__)) } \
    template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> \
    void from_json(const BasicJsonType& nlohmann_json_j, Type& nlohmann_json_t) { const Type nlohmann_json_default_obj{}; NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(MAYE_NLOHMANN_JSON_FROM_WITH_DEFAULT, __VA_ARGS__)) }


#define MAYE_JSON_DEFINE_TYPE(Type, ...)  \
	MAYE_NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Type, __VA_ARGS__)

/*
//int8_t的序列化和反序列化
inline void to_json(njson_t& nlohmann_json_j, const int8_t& nlohmann_json_t) {
	nlohmann_json_j = static_cast<int32_t>(nlohmann_json_t);
}

inline void from_json(const njson_t& nlohmann_json_j, int8_t& nlohmann_json_t) {
	nlohmann_json_t = static_cast<int32_t>(nlohmann_json_j);
}

inline void to_json(njson_t& nlohmann_json_j, const uint8_t& nlohmann_json_t) {
	nlohmann_json_j = static_cast<uint32_t>(nlohmann_json_t);
}

inline void from_json(const njson_t& nlohmann_json_j, uint8_t& nlohmann_json_t) {
	nlohmann_json_t = static_cast<uint32_t>(nlohmann_json_j);
}

inline void to_json(njson_t& nlohmann_json_j, const std::vector<uint8_t>& nlohmann_json_t) {
	for (auto& v : nlohmann_json_t) {
		nlohmann_json_j.push_back(v);
	}
}

inline void from_json(const njson_t& nlohmann_json_j, std::vector<uint8_t>& nlohmann_json_t) {
	for (auto& v : nlohmann_json_j) {
		nlohmann_json_t.push_back(v.get<uint8_t>());
	}
}

inline void to_json(njson_t& nlohmann_json_j, const std::vector<int32_t>& nlohmann_json_t) {
	for (auto& v : nlohmann_json_t) {
		nlohmann_json_j.push_back(v);
	}
}

inline void from_json(const njson_t& nlohmann_json_j, std::vector<int32_t>& nlohmann_json_t) {
	for (auto& v : nlohmann_json_j) {
		nlohmann_json_t.push_back(v);
	}
}*/


