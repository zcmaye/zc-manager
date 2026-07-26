#include "../UserPostDao.h"

class MysqlUserPostDao  : public UserPostDao {
	static Object<Entity> toEntity(const sql::ResultSet* result)
	{
		auto e = std::make_shared<Entity>();
		SafeGetInt(result, e->post_id, "post_id");
		SafeGetInt(result, e->user_id, "user_id");
		return e;
	}

public:
	Object<SysUserPost> selectById(int32_t id, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return Object<SysUserPost>();
	}
	bool insert(const Object<SysUserPost>& e, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return false;
	}
	int32_t update(const Object<SysUserPost>& e, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return 0;
	}
	int32_t deleteByIds(const std::vector<int32_t>& ids, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return 0;
	}
	int countUserPostByPostId(int32_t postId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT COUNT(*) FROM sys_user_post WHERE post_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, postId);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return result->getInt(1);
			}
			return 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	bool deletedUserPostInfo(const Object<SysUserPost>& userPost, const zc::mysql::PooledConnection& con) override
	{
		auto query = "DELETE FROM sys_user_post WHERE post_id = ? AND user_id =?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, userPost->post_id);
			prepared->setInt(2, userPost->user_id);
			return prepared->executeUpdate() > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	bool deletedUserPostInfos(int32_t postId, const std::vector<int32_t> userIds, const zc::mysql::PooledConnection& con) override
	{
		std::string  placeholders;
		for (size_t i = 0; i < userIds.size(); i++) {
			placeholders += '?';
			if (i < userIds.size() - 1) {
				placeholders += ",";
			}
		}

		auto query = "DELETE FROM sys_user_post WHERE post_id = ? AND user_id IN(" + placeholders + ")";

		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1,postId);
			for (size_t i = 0; i < userIds.size(); i++) {
				prepared->setInt(i + 1 + 1, userIds[i]);
			}
			return prepared->executeUpdate();
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	bool batchUserPost(int32_t userId, const std::vector<int32_t> postIds, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Insert("post_id"_c, "user_id"_c)
			.values_for(postIds, [userId](int32_t postId) {
			return std::vector{ format_value(postId),format_value(userId) };
				})
			.into("sys_user_post")
			.to_string();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			return stmt->executeUpdate(query) > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	bool batchUserPost(const std::vector<Object<SysUserPost>>& list, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Insert("post_id"_c, "user_id"_c)
			.values_for(list, [](const Object<SysUserPost>& up) {
			return std::vector{ format_value(up->post_id),format_value(up->user_id) };
				})
			.into("sys_user_post")
			.to_string();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			return stmt->executeUpdate(query) > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	bool deleteUserPostByUserId(int32_t userId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "DELETE FROM sys_user_post WHERE user_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1,userId);
			return prepared->executeUpdate() > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
};

std::shared_ptr<UserPostDao> UserPostDao::createShared()
{
	return std::make_shared<MysqlUserPostDao>();
}

