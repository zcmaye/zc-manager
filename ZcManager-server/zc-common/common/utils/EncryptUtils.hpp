#pragma once

#include "openssl/sha.h"

class EncryptUtils {
public:
	/**
	* SHA256哈希算法
	*/
	static std::string sha256(const std::string& str)
	{
		//加密
		std::array<unsigned char, SHA256_DIGEST_LENGTH> buffer = { 0 };
		if (!SHA256((unsigned char*)str.data(), str.size(), buffer.data())) {
			std::printf("SHA256 failed");
			return "";
		}
		return std::string((char*)buffer.data(), buffer.size());
	}


	/**
	*  将二进制转成十六进制字符串
	*/
	static std::string toHex(const char* str, size_t size, bool upper = false)
	{
		std::string hexString;
		for(size_t i =0;i<size;i++){
			if (upper)
				hexString += std::format("{:02X}", str[i]);
			else
				hexString += std::format("{:02x}", str[i]);
		}
		return hexString;

	}

	static std::string toHex(const std::string& str, bool upper = false)
	{
		return toHex(str.data(), str.size(), upper);
	}


	/**
	* 加密密码
	*/
	static std::string encrpytPassword(const std::string& password)
	{
		auto buffer = EncryptUtils::sha256(password);
		auto hexString = EncryptUtils::toHex(buffer);
		return hexString;
	}

	/**
	* 判断密码是否相同
	* @param rawPassword 原始密码
	* @param encodedPassword 加密后的密码
	* @return bool 
	*/
	static bool matchesPassword(const std::string& rawPassword,const std::string& encodedPassword) {
		return encrpytPassword(rawPassword) == encodedPassword;
	}
};
