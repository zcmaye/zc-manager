#pragma once

#include "framework/controller/BaseController.hpp"

class DeptController : public BaseController
{
public:
	DeptController() {}

	static std::shared_ptr<DeptController> createShared();

	/**
	 * 获取部门列表.
	 */
	virtual int tree(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据部门ID获取部门信息.
	 */
	virtual int getInfo(const HttpContextPtr& ctx) = 0;

	/**
	 * 获取部门选择树
	 */
	virtual int options(const HttpContextPtr& ctx) = 0;

	/**
	 * 添加部门.
	 */
	virtual int add(const HttpContextPtr& ctx) = 0;

	/**
	 * 修改(编辑)部门信息.
	 */
	virtual int edit(const HttpContextPtr& ctx) = 0;

	/**
	 * 根据部门ID删除部门.
	 */
	virtual int remove(const HttpContextPtr& ctx) = 0;
};

