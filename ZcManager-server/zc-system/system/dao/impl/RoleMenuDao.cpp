#include "../RoleMenuDao.h"

class MysqlRoleMenuDao : public RoleMenuDao
{
	static Object<Entity> toEntity(const sql::ResultSet* result)
	{
		auto e = std::make_shared<Entity>();
		SafeGetInt(result, e->role_id, "role_id");
		SafeGetInt(result, e->menu_id, "menu_id");
		return e;
	}

public:
	Object<SysRoleMenu> selectById(int32_t id, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return Object<SysRoleMenu>();
	}

	bool insert(const Object<SysRoleMenu>& e, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return false;
	}

	int32_t update(const Object<SysRoleMenu>& e, const zc::mysql::PooledConnection& con) override
	{
		throw ServiceError("未实现...");
		return 0;
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

		auto query = "DELETE FROM sys_role_menu WHERE role_id IN(" + placeholders + ")";

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

	bool batchInsertRoleMenu(int32_t roleId, const std::vector<int>& menuIds, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Insert("role_id"_c,"menu_id"_c)
			.values_for(menuIds, [roleId](int menuId) {
					return std::vector{format_value(roleId),format_value(menuId)};
				})
			.into("sys_role_menu")
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

};


DAO_IMPL(RoleMenuDao, MysqlRoleMenuDao)


