#pragma once

#define ENUM_V(NAME,VALUE,DESC) NAME = VALUE

enum ErrorCode{
	ENUM_V(Success,200,"成功"),
	ENUM_V(Warning,201,"警告"),
	ENUM_V(Error, 500,"错误")
};
