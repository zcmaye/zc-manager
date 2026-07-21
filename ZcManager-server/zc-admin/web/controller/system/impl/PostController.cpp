#include "../PostController.h"

#include "system/service/PostService.h"
#include "common/constants/UserConstants.hpp"
#include "common/utils/StringUtils.h"
#include "web/utils/QueryParamsMapping.hpp"

class PostControllerImpl : public PostController
{
	PostService m_postService;
public:
	void registerRouters(hv::HttpService* router) override
	{
		ENDPOINT(router, GET, "/post/list", list);
		ENDPOINT(router, POST, "/post/add", add);
		ENDPOINT(router, PUT, "/post/edit", edit);
		ENDPOINT(router, Delete, "/post/{postIds}", remove);
		ENDPOINT(router, GET, "/post/options", options);

		//这个端点必须放到最后，否则会覆盖掉/post/list
		ENDPOINT(router, GET, "/post/{postId}", getInfo);
	}
	int list(const HttpContextPtr& ctx) override
	{
		//获取参数
		auto post = QueryParamsMapping::mapTo<SysPost>(ctx->params());

		//调用服务
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_postService.selectPostList(post, post->params.page, post->params.pageSize)));
	}

	int getInfo(const HttpContextPtr& ctx) override
	{
		//检查参数
		int32_t postId;
		if (auto v = ctx->param("postId"); v.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少岗位ID"));
		}
		else {
			postId = std::stoi(v);
			if (postId <= 0) {
				return sendResult(ctx, HTTP_STATUS_OK, Result::error("缺少岗位ID"));
			}
		}

		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_postService.selectPostById(postId)));
	}

	int add(const HttpContextPtr& ctx) override
	{
		auto post = std::make_shared<SysPost>();
		*post = hv::Json::parse(ctx->body());

		//检查参数
		if (!post->post_code) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增岗位失败，缺少岗位编码~"));
		}
		else if (!post->post_name) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增岗位失败，缺少岗位名称~"));
		}

		//检查唯一性
		if (!m_postService.checkPostCodeUnique(post)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增岗位'" + *post->post_name + "'失败，岗位编码已存在~"));
		}
		else if (!m_postService.checkPostNameUnique(post)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增岗位'" + *post->post_name + "'失败，岗位名称已存在~"));
		}

		//创建人
		post->create_by = getUsername();

		//执行插入
		m_postService.insertPost(post);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("岗位新增成功~"));
	}

	int edit(const HttpContextPtr& ctx) override
	{
		auto post = std::make_shared<SysPost>();
		*post = hv::Json::parse(ctx->body());

		//检查参数
		if (!post->post_id) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("修改岗位失败，缺少岗位ID~"));
		}

		//检查唯一性
		if (post->post_code && !m_postService.checkPostCodeUnique(post)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增岗位失败，岗位编码已存在~"));
		}
		else if (post->post_name && !m_postService.checkPostNameUnique(post)) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::success("新增岗位'" + *post->post_name + "'失败，岗位名称已存在~"));
		}

		//创建人
		post->update_by = getUsername();

		//执行插入
		m_postService.updatePost(post);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("岗位更新成功~"));
	}

	
	int remove(const HttpContextPtr& ctx) override
	{
		auto ids_str = ctx->param("postIds");
		if (ids_str.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("删除失败，缺少岗位ID~"));
		}

		auto ids = StringUtils::split<int>(ids_str, ',');
		if (ids.empty()) {
			return sendResult(ctx, HTTP_STATUS_OK, Result::error("删除失败，缺少岗位ID~"));
		}

		//删除
		m_postService.deletePosts(ids);
		return sendResult(ctx, HTTP_STATUS_OK, Result::success("删除成功~"));
	}

	int options(const HttpContextPtr& ctx) override
	{
		return sendResult(ctx, HTTP_STATUS_OK, Result::success(m_postService.selectPostOptions()));
	}
};

std::shared_ptr<PostController> PostController::createShared()
{
	return std::make_shared<PostControllerImpl>();
}
