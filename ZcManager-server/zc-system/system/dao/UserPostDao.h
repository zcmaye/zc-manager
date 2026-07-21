#pragma once

#include "common/domain/dao/BaseDao.hpp"

#include "system/domain/entity/SysPost.hpp"
#include "system/domain/entity/SysUserPost.hpp"

class UserPostDao  : public BaseDao<SysUserPost,UserPostDao>
{
	DAO_INIT(UserPostDao);
public:
	/**
	 * 统计指定岗位分配给了几个用户.
	 */
	virtual int countUserPostByPostId(int32_t postId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 取消授权用户岗位.
	 */
	virtual bool deletedUserPostInfo(const Object<SysUserPost>& userPost,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 批量取消授权用户岗位.
	 */
	virtual bool deletedUserPostInfos(int32_t postId, const std::vector<int32_t> userIds,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	/**
	 * 批量插入用户岗位关联数据
	 */
	virtual bool batchUserPost(int32_t userId, const std::vector<int32_t> postIds,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
public:
	virtual bool batchUserPost(const std::vector<Object<SysUserPost>>& list, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;

	virtual bool deleteUserPostByUserId(int32_t userId, const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION) = 0;
};

