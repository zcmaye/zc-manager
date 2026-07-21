#pragma once

#include "common/domain/dao/BaseDao.hpp"

#include "system/domain/entity/SysPost.hpp"

class PostDao : public BaseDao<SysPost,PostDao>
{
	DAO_INIT(PostDao);
public:

	/**
	 * 查询岗位.
	 */
	virtual ObjectList<SysPost> selectPostList(const Object<SysPost>& post = std::make_shared<SysPost>(), const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 查询岗位.
	 */
	virtual Object<SysPost> selectPostByPostCode(const std::string& postCode, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
	virtual Object<SysPost> selectPostByPostName(const std::string& postName, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 根据用户所在的岗位列表.
	 */
	virtual std::vector<int32_t> selectPostByUserId(int32_t userId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
};

