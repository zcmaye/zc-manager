#include "PostService.h"
#include "../dao/PostDao.h"
#include "common/utils/PageUtils.hpp"

PostService::PostService()
	:m_postDao(PostDao::createShared())
{}

PostService::~PostService()
{}

std::shared_ptr<PagingVo<SysPost>> PostService::selectPostList(const Object<SysPost>&post, int32_t page, int32_t pageSize)
{
	auto list = m_postDao->selectPostList(post);
	return PageUtils(page, pageSize).getPageData(list);
}

ObjectList<SysPost> PostService::selectPostAll(const zc::mysql::PooledConnection& con)
{
	return m_postDao->selectPostList();
}

ObjectList<PostOptions> PostService::selectPostOptions() {
	ObjectList<PostOptions> options;
	auto posts = m_postDao->selectPostList();
	for (auto& post : posts) {
		auto opt = std::make_shared<PostOptions>();
		opt->post_id = *post->post_id;
		opt->post_code = *post->post_code;
		opt->post_name = *post->post_name;
		options.push_back(opt);
	}
	return options;
}

Object<SysPost> PostService::selectPostById(int32_t postId)
{
	return m_postDao->selectById(postId);
}

Object<SysPost> PostService::selectPostByPostCode(const std::string& postCode)
{
	return m_postDao->selectPostByPostCode(postCode);
}

std::vector<int32_t> PostService::selectPostByUserId(int32_t userId,const zc::mysql::PooledConnection& con)
{
	return m_postDao->selectPostByUserId(userId,con);
}

bool PostService::insertPost(const Object<SysPost>& post)
{
	return m_postDao->insert(post);
}


bool  PostService::deletePost(const Object<SysPost>& post)
{
	return m_postDao->deleteByIds({ *post->post_id });
}

bool PostService::deletePosts(const std::vector<int>& postIds)
{
	return m_postDao->deleteByIds(postIds);
}

bool PostService::updatePost(const Object<SysPost>& post)
{
	return m_postDao->update(post);
}

bool PostService::checkPostCodeUnique(const Object<SysPost>& post)
{
	auto postId = post->post_id.value_or(-1);

	auto info = m_postDao->selectPostByPostCode(*post->post_code);
	if (info && *info->post_id != postId) {
		return false;
	}
	return true;
}

bool PostService::checkPostNameUnique(const Object<SysPost>& post)
{
	auto postId = post->post_id.value_or(-1);

	auto info = m_postDao->selectPostByPostName(*post->post_name);
	if (info && *info->post_id != postId) {
		return false;
	}
	return true;

}

