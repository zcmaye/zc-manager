#include "../DeptDao.h"

class MysqlDeptDao : public DeptDao
{
	static Object<Entity> toEntity(const sql::ResultSet* result)
	{
		auto e = std::make_shared<Entity>();
		SafeGetInt(result, e->dept_id, "dept_id");
		SafeGetInt(result, e->parent_id, "parent_id");
		SafeGetString(result,e->ancestors,"ancestors");
		SafeGetString(result,e->dept_name,"dept_name");
		SafeGetInt(result,e->order_num,"order_num");
		SafeGetString(result,e->leader,"leader");
		SafeGetString(result,e->phone,"phone");
		SafeGetString(result,e->email,"email");
		SafeGetInt(result,e->is_deleted,"is_deleted");
		SafeGetInt(result,e->is_active,"is_active");
		SafeGetString(result,e->create_time,"create_time");
		SafeGetString(result,e->create_by,"create_by");
		SafeGetString(result,e->update_time,"update_time");
		SafeGetString(result,e->update_by,"update_by");
		SafeGetString(result,e->remark,"remark");
		return e;
	}
public:
	Object<SysDept> selectById(int32_t id, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_dept WHERE dept_id = ?";
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

	bool insert(const Object<SysDept>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Insert("parent_id"_c, "ancestors"_c, "dept_name"_c, "order_num"_c,
			"leader"_c, "phone"_c, "email"_c,"is_active"_c,"create_time"_c, "create_by"_c, "remark"_c)
			.values(e->parent_id,e->ancestors,e->dept_name,e->order_num,
				e->leader, e->phone, e->email, e->is_active,fun::now(), e->create_by, e->remark)
			.into("sys_dept")
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

	int32_t update(const Object<SysDept>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Update("sys_dept")
			.set("parent_id"_c = e->parent_id)
			("ancestors"_c = e->ancestors)
			("dept_name"_c = e->dept_name)
			("order_num"_c = e->order_num)
			("leader"_c = e->leader)
			("email"_c = e->email)
			("phone"_c = e->phone)
			("update_by"_c = e->update_by)
			("update_time"_c = fun::now())
			("remark"_c = e->remark)
			("is_deleted"_c = e->is_deleted)
			.where("dept_id"_c == e->dept_id)
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

		auto query = "UPDATE sys_dept SET is_deleted = TRUE WHERE dept_id IN(" + placeholders + ")";

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

	ObjectList<SysDept> selectDeptList(const Object<SysDept>& dept, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Select(all)
			.from("sys_dept")
			.where("dept_name"_c.like(dept->dept_name) and "is_active"_c == dept->is_active	and "is_deleted"_c == false)
			.order_by("dept_id"_c.desc(), "create_by"_c.desc())
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

	ObjectList<SysDept> selectChildrenDeptById(int32_t deptId, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_dept WHERE FIND_IN_SET(?,ancestors) AND is_deleted=0";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, deptId);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
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

	bool updateDeptChildren(const ObjectList<SysDept>& children, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto col = fun::case_end_for("dept_id", children, [](const Object<SysDept>& dept) {
				return std::make_pair(
					format_value(dept->dept_id),
					format_value(dept->ancestors)
				);
			});

		//拼接所有id
		std::vector<int32_t> ids;
		for (size_t i = 0; i < children.size(); i++) {
			ids.push_back(*children[i]->dept_id);
		}

		auto query = Update("sys_dept")
			.set("ancestors"_c = col)
			.where("dept_id"_c.in(ids))
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

	bool updateParentDeptStatus(const std::vector<int32_t>& parendIds, bool isActive, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::sqlbuilder;
		using namespace zc::sqlbuilder::field_literals;

		auto query = Update("sys_dept")
			.set("is_active"_c = isActive)
			.where("dept_id"_c.in(parendIds))
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

	Object<SysDept> selectByName(const std::string& name, int32_t parentId, const zc::mysql::PooledConnection& con)override
	{
		auto query = "SELECT * FROM sys_dept WHERE dept_name = ? AND parent_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, name);
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
	bool hasChildByDeptId(int32_t deptId, const zc::mysql::PooledConnection& con)
	{
		auto query = "SELECT COUNT(*) FROM sys_dept WHERE parent_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, deptId); 

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
	bool checkDeptExistUser(int32_t deptId, const zc::mysql::PooledConnection& con)
	{
		auto query = "SELECT COUNT(*) FROM sys_user WHERE dept_id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, deptId); 

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
	int32_t selectNormalChildrenByDeptId(int32_t deptId, const zc::mysql::PooledConnection& con)
	{
		auto query = "SELECT COUNT(*) FROM sys_dept WHERE is_active = 1 AND FIND_IN_SET(?,ancestors)";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, deptId); 

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return result->getInt(1);
			}
			return 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
		return 0;
	}
};

DAO_IMPL(DeptDao, MysqlDeptDao)

