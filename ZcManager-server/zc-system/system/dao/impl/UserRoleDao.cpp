#include "../UserRoleDao.h"

class MysqlUserRoleDao : public UserRoleDao
{
	static Object<Entity> toEntity(const sql::ResultSet* result)
	{
		auto e = std::make_shared<Entity>();
		SafeGetInt(result, e->role_id, "role_id");
		SafeGetInt(result, e->user_id, "user_id");
		return e;
	}
public:
	Object<Entity> selectById(int32_t id, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return Object<Entity>();
	}

	bool insert(const Object<Entity>& e, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return false;
	}

	int32_t update(const Object<Entity>& e, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return 0;
	}

	int32_t deleteByIds(const std::vector<int32_t>& ids, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return 0;
	}

	int countUserRoleByRoleId(int32_t roleId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT COUNT(*) FROM sys_user_role WHERE role_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, roleId);

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

	bool deletedUserRoleInfo(const Object<SysUserRole>& userRole, const zc::mysql::PooledConnection& con) override
	{
		auto query = "DELETE FROM sys_user_role WHERE user_id = ? AND role_id =?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, userRole->user_id);
			prepared->setInt(2, userRole->role_id);
			return prepared->executeUpdate() > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	bool deletedUserRoleInfos(int32_t roleId, const std::vector<int32_t> userIds, const zc::mysql::PooledConnection& con) override
	{
		std::string  placeholders;
		for (size_t i = 0; i < userIds.size(); i++) {
			placeholders += '?';
			if (i < userIds.size() - 1) {
				placeholders += ",";
			}
		}

		auto query = "DELETE FROM sys_user_role WHERE role_id = ? AND user_id IN(" + placeholders + ")";

		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, roleId);
			for (size_t i = 0; i < userIds.size(); i++) {
				prepared->setInt(i + 1 + 1, userIds[i]);
			}
			return prepared->executeUpdate();
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	bool batchUserRole(int32_t roleId, const std::vector<int32_t> userIds, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Insert("role_id"_c, "user_id"_c)
			.values_for(userIds, [roleId](int32_t userId) {
			return std::vector{ format_value(roleId),format_value(userId) };
				})
			.into("sys_user_role")
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

	bool batchUserRole(const ObjectList<SysUserRole>& list, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Insert("role_id"_c, "user_id"_c)
			.values_for(list, [](const Object<SysUserRole>& ur) {
			return std::vector{ format_value(ur->role_id),format_value(ur->user_id) };
				})
			.into("sys_user_role")
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

	bool deleteUserRoleByUserId(int32_t userId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "DELETE FROM sys_user_role WHERE user_id = ?";
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


DAO_IMPL(UserRoleDao, MysqlUserRoleDao)

