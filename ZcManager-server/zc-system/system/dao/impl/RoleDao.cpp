#include "../RoleDao.h"


class MysqlRoleDao : public RoleDao {
	static Object<Entity> toEntity(const sql::ResultSet* result)
	{
		auto e = std::make_shared<Entity>();
		SafeGetInt(result, e->role_id, "role_id");
		SafeGetString(result, e->role_name, "role_name");
		SafeGetString(result,e->role_key,"role_key");
		SafeGetInt(result,e->role_sort,"role_sort");
		SafeGetInt(result,e->is_active,"is_active");
		SafeGetString(result,e->create_time,"create_time");
		SafeGetString(result,e->create_by,"create_by");
		SafeGetString(result,e->update_time,"update_time");
		SafeGetString(result,e->update_by,"update_by");
		SafeGetString(result,e->remark,"remark");
		return e;
	}
public:
	Object<SysRole> selectById(int32_t id, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_role WHERE role_id = ?";
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

	bool insert(const Object<SysRole>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Insert("role_name"_c, "role_key"_c, "role_sort"_c, "is_active"_c, "create_time"_c, "create_by"_c, "remark"_c)
			.values(e->role_name,e->role_key,e->role_sort,e->is_active, fun::now(), e->create_by, e->remark)
			.into("sys_role")
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

	int32_t update(const Object<SysRole>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Update("sys_role")
			.set("role_name"_c = e->role_name)
			("role_key"_c = e->role_key)
			("role_sort"_c = e->role_sort)
			("is_active"_c = e->is_active)
			("update_by"_c = e->update_by)
			("update_time"_c = fun::now())
			("remark"_c = e->remark)
			.where("role_id"_c == e->role_id)
			.to_string();

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

		auto query = "UPDATE sys_role SET is_deleted = true WHERE role_id IN(" + placeholders + ")";

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

	ObjectList<SysRole> selectRoleList(const Object<SysRole>& role, int32_t page, int32_t pageSize, const zc::mysql::PooledConnection& con)
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Select(all)
			.from("sys_role")
			.where("role_name"_c.like(role->role_name) and "role_key"_c.like(role->role_key) 
				and "is_active"_c == role->is_active
				and "is_deleted"_c == false 
				and "create_time"_c.between_and(role->params.begTime, role->params.endTime)
			)
			.order_by("role_sort"_c, "create_by"_c.desc())
			.limit(role->params.offset(), role->params.limit())
			.to_string();

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

	Object<SysRole> selectRoleByName(const std::string& roleName, const zc::mysql::PooledConnection& con) const
	{
		auto query = "SELECT * FROM sys_role WHERE role_name = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, roleName);

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

	Object<SysRole> selectRoleByKey(const std::string& roleKey, const zc::mysql::PooledConnection& con) const
	{
		auto query = "SELECT * FROM sys_role WHERE role_key = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, roleKey);

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

	bool updateRoleStatus(const Object<SysRole>& role, const zc::mysql::PooledConnection& con) override
	{
		auto query = "UPDATE sys_role SET is_active = ? WHERE role_id = ? ";

		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1,*role->is_active);
			prepared->setInt(2,*role->role_id);
			return prepared->executeUpdate();
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	ObjectList<SysRole> selectRoleListByUserId(int32_t userId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_role LEFT JOIN sys_user_role USING(role_id) WHERE is_active=1  AND is_deleted=0 AND user_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, userId);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			ObjectList<SysRole> list;
			while(result->next()) {
				list.push_back(toEntity(result.get()));
			}
			return list;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
};

DAO_IMPL(RoleDao, MysqlRoleDao)

