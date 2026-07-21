#pragma once

#include "ErrorCode.hpp"
#include "hv/json.hpp"

namespace details {
	template <typename T, typename = void>
	struct is_string : std::false_type {};

	// 匹配std::string（包括wstring等basic_string变体）
	template <typename CharT, typename Traits, typename Alloc>
	struct is_string<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};

	template <typename CharT, typename Traits>
	struct is_string<std::basic_string_view<CharT, Traits>> : std::true_type {};

	// 匹配const char*
	template <>
	struct is_string<const char*> : std::true_type {};

	// 匹配char*
	template <>
	struct is_string<char*> : std::true_type {};

	// 匹配char[N]
	template <size_t N>
	struct is_string<char[N]> : std::true_type {};

	// 字符串特性别名
	template <typename T>
	inline constexpr bool is_string_v = is_string<T>::value;

	// ========== 第三步：定义map判定特性 ==========
	template <typename T, typename = void>
	struct is_map : std::false_type {};

	// 匹配std::map
	template <class _Kty, class _Ty, class _Pr, class _Alloc>
	struct is_map<std::map<_Kty, _Ty, _Pr, _Alloc>> : std::true_type {};

	// 匹配std::unordered_map
	template <class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
	struct is_map<std::unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>> : std::true_type {};

	// 匹配std::vector<std::pair<?,?>>
	template <class _Kty, class _Vty>
	struct is_map<std::vector<std::pair<_Kty, _Vty>>> : std::true_type {};

	// 字符串特性别名
	template <typename T>
	inline constexpr bool is_map_v = is_map<T>::value;
}

class Result {
private:
	hv::Json m_object;
public:
	template<typename T, std::enable_if_t<!details::is_string_v<T>, int> = 0>
	Result(ErrorCode code, const std::string& message, const T& data)
		: m_object{
			{ "code",code },
			{ "msg",message },
			{ "data",data }
		}
	{}

	const hv::Json& toJson()const { return m_object; }

	void setCode(ErrorCode code) { m_object["code"] = code; }
	ErrorCode code()const { return m_object["code"].get<ErrorCode>(); }

	void setMessage(const std::string& message) { m_object["msg"] = message; }
	std::string message()const { return m_object["msg"].get<std::string>(); }

	void setData(const hv::Json& data) { m_object["data"] = data; }
	template<typename T>
	T data()const { return m_object["data"].get<T>(); }

public:
	/**
	 * 返回成功消息.
	 *
	 * @param message 消息内容
	 * @param data    数据对象
	 * @return 成功消息
	 */
	template<typename T>
	static Result success(const std::string& message, const T& data) {
		return Result(ErrorCode::Success, message, data);
	}

	static Result success(const std::string& message) {
		return Result(ErrorCode::Success, message, nullptr);
	}

	template<typename T, std::enable_if_t<!details::is_string_v<T>, int> = 0>
	static Result success(const T& data) {
		return Result(ErrorCode::Success, "success", data);
	}

	static Result success() {

		return Result(ErrorCode::Success, "success", nullptr);
	}

	/**
	 * 返回警告息.
	 *
	 * @param message 消息内容
	 * @param data    数据对象
	 * @return 警告消息
	 */
	template<typename T>
	static Result warning(const std::string& message,const T& data) {
		return Result(ErrorCode::Warning, message, data);
	}

	static Result warning(const std::string& message) {
		return Result(ErrorCode::Warning, message, nullptr);
	}

	template<typename T, std::enable_if_t<!details::is_string_v<T>, int> = 0>
	static Result warning(const T& data) {
		return Result(ErrorCode::Warning, "warning", data);
	}

	static Result warning() {
		return Result(ErrorCode::Warning, "warning", nullptr);
	}

	/**
	 * 返回错误息.
	 *
	 * @param message 消息内容
	 * @param data    数据对象
	 * @return 错误消息
	 */
	template<typename T>
	static Result error(const std::string& message,const T& data) {
		return Result(ErrorCode::Error, message, data);
	}

	static Result error(const std::string& message) {
		return Result(ErrorCode::Error, message, nullptr);
	}

	template<typename T, std::enable_if_t<!details::is_string_v<T>, int> = 0>
	static Result error(const T& data) {
		return Result(ErrorCode::Error, "error", data);
	}

	static Result error() {
		return Result(ErrorCode::Error, "error", nullptr);
	}

	template<typename T>
	Result& put(const std::string& key, const T& value) {
		m_object[key] = value;
		return *this;
	}
};
