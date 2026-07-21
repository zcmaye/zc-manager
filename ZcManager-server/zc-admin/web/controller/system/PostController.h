#pragma once

#include "framework/controller/BaseController.hpp"

class PostController : public BaseController
{
public:
	PostController() {}

	static std::shared_ptr<PostController> createShared();

	/**
	 * 获取岗位列表.
	 */
	virtual int list(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据岗位ID获取岗位信息.
	 */
	virtual int getInfo(const HttpContextPtr& ctx) = 0;

	/**
	 * 添加岗位.
	 */
	virtual int add(const HttpContextPtr& ctx) = 0;

	/**
	 * 修改(编辑)岗位信息.
	 */
	virtual int edit(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据岗位ID删除岗位.
	 */
	virtual int remove(const HttpContextPtr& ctx) = 0;

	/**
	 * 岗位选择.
	 */
	virtual int options(const HttpContextPtr& ctx) = 0;
};

