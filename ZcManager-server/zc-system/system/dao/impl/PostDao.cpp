#include "../PostDao.h"


class MysqlPostDao : public PostDao
{
	static Object<Entity> toEntity(const sql::ResultSet* result)
	{
		auto e = std::make_shared<Entity>();
		SafeGetInt(result, e->post_id, "post_id");
		SafeGetString(result, e->post_code, "post_code");
		SafeGetString(result,e->post_name,"post_name");
		SafeGetInt(result,e->post_sort,"post_sort");
		SafeGetInt(result,e->is_active,"is_active");
		SafeGetString(result,e->create_time,"create_time");
		SafeGetString(result,e->create_by,"create_by");
		SafeGetString(result,e->update_time,"update_time");
		SafeGetString(result,e->update_by,"update_by");
		SafeGetString(result,e->remark,"remark");
		return e;
	}

public:
	Object<SysPost> selectById(int32_t id, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_post WHERE post_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, id);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return toEntity(result.get());
			}
			return {};
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	bool insert(const Object<SysPost>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Insert("post_code"_c, "post_name"_c, "post_sort"_c, "is_active"_c, "create_time"_c,"create_by"_c, "remark"_c)
			.values(e->post_code, e->post_name, e->post_sort, e->is_active, now(), e->create_by, e->remark)
			.into("sys_post")
			.sql();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			return stmt->executeUpdate(query) > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	int32_t update(const Object<SysPost>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Update("sys_post")
			.set("post_name"_c = e->post_name)
			    ("post_sort"_c = e->post_sort)
			    ("is_active"_c = e->is_active)
			    ("update_by"_c = e->update_by)
			    ("update_time"_c = zc::tool::sql::now())
			    ("remark"_c = e->remark)
			.where("post_id"_c == e->post_id)
			.sql();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			return stmt->executeUpdate(query);
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	int32_t deleteByIds(const std::vector<int32_t>& ids, const zc::mysql::PooledConnection& con) override
	{
		std::string  placeholders;
		for (size_t i = 0; i < ids.size(); i++) {
			placeholders += '?';
			if (i < ids.size() - 1) {
				placeholders += ",";
			}
		}

		auto query = "DELETE FROM sys_post WHERE post_id IN(" + placeholders + ")";

		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			for (size_t i = 0; i < ids.size(); i++) {
				prepared->setInt(i + 1, ids[i]);
			}
			return prepared->executeUpdate();
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
public:
	ObjectList<SysPost> selectPostList(const Object<SysPost>& post,const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Select(all)
			.from("sys_post")
			.where("post_name"_c.like(post->post_name) and "post_code"_c.like(post->post_code) and "is_active"_c == post->is_active)
			.order_by("post_sort"_c, "create_by"_c.desc())
			.sql();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());

			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			ObjectList<Entity> list;
			while(result->next()) {
				list.push_back(std::move(toEntity(result.get())));
			}
			return list;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	Object<SysPost> selectPostByPostCode(const std::string& postCode, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_post WHERE post_code = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, postCode);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return toEntity(result.get());
			}
			return {};
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	Object<SysPost> selectPostByPostName(const std::string& postName, const zc::mysql::PooledConnection& con)
	{
		auto query = "SELECT * FROM sys_post WHERE post_name = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, postName);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return toEntity(result.get());
			}
			return {};
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	std::vector<int32_t> selectPostByUserId(int32_t userId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT post_id FROM sys_user_post WHERE user_id=?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, userId);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			std::vector<int32_t> ids;
			while (result->next()) {
				ids.push_back(result->getInt(1));
			}
			return ids;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
};

DAO_IMPL(PostDao, MysqlPostDao)

