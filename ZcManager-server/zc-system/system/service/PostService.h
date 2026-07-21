#pragma once

#include "common/domain/vo/PagingVo.hpp"
#include "common/domain/vo/PostOptions.hpp"

#include "system/domain/entity/SysPost.hpp"
#include "framework/context/ContextHolder.h"

class PostDao;

class PostService{
public:
	PostService();
	~PostService();

	/**
	 * 查询岗位.
	 */
	std::shared_ptr<PagingVo<SysPost>> selectPostList(const Object<SysPost>& post, int32_t page = 1, int32_t pageSize = 10);
	ObjectList<SysPost> selectPostAll(const zc::mysql::PooledConnection& con);

	ObjectList<PostOptions> selectPostOptions();

	/**
	 * 查询岗位.
	 */
	Object<SysPost> selectPostById(int32_t postId);
	Object<SysPost> selectPostByPostCode(const std::string& postCode);
	std::vector<int32_t> selectPostByUserId(int32_t userId,const zc::mysql::PooledConnection& con = DEFAULT_CONNECTION);

	/**
	 * 插入.
	 */
	bool insertPost(const Object<SysPost>& post);

	/**
	 * 删除.
	 */
	bool deletePost(const Object<SysPost>& post);

	/**
	 * 删除.
	 */
	bool deletePosts(const std::vector<int>& postIds);

	/*
	* 更新
	*/
	bool updatePost(const Object<SysPost>& post);

	/**
	 * 检查岗位编码是否唯一.
	 */
	bool checkPostCodeUnique(const Object<SysPost>& post);


	/**
	 * 检查岗位名称是否唯一.
	 */
	bool checkPostNameUnique(const Object<SysPost>& post);
private:
	std::shared_ptr<PostDao> m_postDao;
};
