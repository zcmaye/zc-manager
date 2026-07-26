#include "../MenuDao.h"


class MysqlMenuDao : public MenuDao {
	static Object<Entity> toEntity(const sql::ResultSet* result)
	{
		auto e = std::make_shared<Entity>();
		SafeGetInt(result, e->menu_id, "menu_id");
		SafeGetString(result, e->menu_name, "menu_name");
		SafeGetInt(result,e->parent_id,"parent_id");
		SafeGetInt(result,e->order_num,"order_num");
		SafeGetString(result,e->path,"path");
		SafeGetString(result,e->component,"component");
		SafeGetInt(result,e->is_frame,"is_frame");
		SafeGetString(result,e->menu_type,"menu_type");
		SafeGetInt(result,e->is_visible,"is_visible");
		SafeGetInt(result,e->is_active,"is_active");
		SafeGetString(result,e->perms,"perms");
		SafeGetString(result,e->icon,"icon");
		SafeGetString(result,e->create_time,"create_time");
		SafeGetString(result,e->create_by,"create_by");
		SafeGetString(result,e->update_time,"update_time");
		SafeGetString(result,e->update_by,"update_by");
		SafeGetString(result,e->remark,"remark");
		return e;
	}

public:
	Object<SysMenu> selectById(int32_t id, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_menu WHERE menu_id = ?";
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

	bool insert(const Object<SysMenu>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Insert("menu_name"_c, "parent_id"_c, "order_num"_c, "path"_c, "component"_c,
			"is_frame"_c, "menu_type"_c, "is_visible"_c, "is_active"_c, "perms"_c, "icon"_c,"create_time"_c, "create_by"_c, "remark"_c)
			.values(e->menu_name,e->parent_id,e->order_num,e->path,e->component,
				e->is_frame, e->menu_type, e->is_visible, e->is_active,e->perms,e->icon,fun::now(), e->create_by, e->remark)
			.into("sys_menu")
			.to_string();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			return stmt->executeUpdate(query) > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}

		return false;
	}

	int32_t update(const Object<SysMenu>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Update("sys_menu")
			.set("menu_name"_c = e->menu_name)
			("parent_id"_c = e->parent_id)
			("order_num"_c = e->order_num)
			("path"_c = e->path)
			("component"_c = e->component)
			("is_frame"_c = e->is_frame)
			("menu_type"_c = e->menu_type)
			("is_visible"_c = e->is_visible)
			("is_active"_c = e->is_active)
			("perms"_c = e->perms)
			("icon"_c = e->icon)
			("update_time"_c = fun::now())
			("remark"_c = e->remark)
			.where("menu_id"_c == e->menu_id)
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

		auto query = "DELETE FROM sys_menu WHERE menu_id IN(" + placeholders + ")";

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

	ObjectList<SysMenu> getMenuList(const Object<SysMenu>& menu, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Select(all)
			.from("sys_menu")
			.where("menu_name"_c.like(menu->menu_name) and "is_active"_c == menu->is_active)
			.order_by("order_num"_c.asc(), "create_by"_c.desc())
			.to_string();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			ObjectList<Entity> list;
			while (result->next()) {
				list.push_back(std::move(toEntity(result.get())));
			}
			return list;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	ObjectList<SysMenu> getMenuListByUserId(int32_t userId, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Select("menu_id"_c,"parent_id"_c,"menu_name"_c,"path"_c,"component"_c,
			"is_visible"_c,"is_active"_c,"IFNULL(perms,'')"_c.as("perms"),"is_frame"_c,"menu_type"_c,
			"icon"_c,"order_num"_c,"create_time"_c,"user_id"_c)
			.from("sys_menu")
			.left_join("sys_role_menu") .using_("menu_id"_c)
			.left_join("sys_user_role") .using_("role_id"_c)
			.where("menu_type"_c.in(std::vector{ "D","M","B"}) and "is_active"_c == true and "user_id"_c == userId)
			.order_by("parent_id"_c, "order_num"_c)
			.to_string();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			ObjectList<Entity> list;
			while (result->next()) {
				list.push_back(std::move(toEntity(result.get())));
			}
			return list;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	ObjectList<SysMenu> getMenuListByUserId(const Object<SysMenu>& menu, int32_t userId, const zc::mysql::PooledConnection& con)
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Select("menu_id"_c,"parent_id"_c,"menu_name"_c,"path"_c,"component"_c,
			"is_visible"_c,"is_active"_c,"IFNULL(perms,'')"_c.as("perms"),"is_frame"_c,"menu_type"_c,
			"icon"_c,"order_num"_c,"create_time"_c,"user_id"_c)
			.from("sys_menu")
			.left_join("sys_role_menu") .using_("menu_id"_c)
			.left_join("sys_user_role") .using_("role_id"_c)
			.where("menu_type"_c.in(std::vector{ "D","M","B"}) and "is_active"_c == menu->is_active and "user_id"_c == userId and "menu_name"_c == menu->menu_name)
			.order_by("parent_id"_c, "order_num"_c)
			.to_string();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			ObjectList<Entity> list;
			while (result->next()) {
				list.push_back(std::move(toEntity(result.get())));
			}
			return list;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}


	std::set<int32_t> getMenuListByRoleId(int32_t roleId, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Select("menu_id"_c)
			.from("sys_menu")
			.left_join("sys_role_menu") .using_("menu_id"_c)
			.where("menu_type"_c.in(std::vector{ "D","M","B" }) and 
				"role_id"_c == roleId)
			.to_string();
		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());
			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			std::set<int32_t> ids;
			while (result->next()) {
				ids.insert(result->getInt(1));
			}
			return ids;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	std::set<std::string> selectMenuPermsByRoleId(int32_t roleId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT DISTINCT perms FROM sys_menu LEFT JOIN sys_role_menu USING(menu_id) WHERE is_active = 1 AND role_id = ? AND LENGTH(perms)";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, roleId);
			std::set<std::string> perms;
			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			while(result->next()) {
				perms.insert(result->getString(1));
			}
			return perms;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	std::set<std::string> selectMenuPermsByUserId(int32_t userId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT DISTINCT perms FROM sys_menu LEFT JOIN sys_role_menu USING(menu_id) LEFT JOIN sys_user_role USING(role_id) WHERE is_active = 1 AND user_id = ? AND LENGTH(perms)";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, userId);
			std::set<std::string> perms;
			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			while(result->next()) {
				perms.insert(result->getString(1));
			}
			return perms;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	Object<SysMenu> selectByName(const std::string& menuName, int32_t parentId, const zc::mysql::PooledConnection& con)
	{
		auto query = "SELECT * FROM sys_menu WHERE menu_name = ? AND parent_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, menuName);
			prepared->setInt(2, parentId);

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

	bool hasChildByMenuId(int32_t menuId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT COUNT(*) FROM sys_menu WHERE parent_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, menuId);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return result->getInt(1) > 0;
			}
			return false;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
		return false;
	}

	bool checkMenuExistRole(int32_t menuId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT COUNT(*) FROM sys_role_menu WHERE menu_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, menuId);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return result->getInt(1) > 0;
			}
			return false;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
		return false;
	}
};

DAO_IMPL(MenuDao, MysqlMenuDao)

