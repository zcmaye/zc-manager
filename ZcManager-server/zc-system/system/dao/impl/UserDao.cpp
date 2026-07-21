#include "../UserDao.h"

#include "common/utils/StringUtils.h"

class MysqlUserDao : public UserDao {
	static Object<SysUser> toEntity(const sql::ResultSet* result)
	{
		auto user= std::make_shared<SysUser>();
		SafeGetInt(result, user->user_id, "id");
		SafeGetInt(result, user->dept_id, "dept_id");
		SafeGetString(result,user->user_name,"user_name");
		SafeGetString(result,user->nick_name,"nick_name");
		SafeGetString(result,user->password,"password");
		SafeGetString(result,user->phone_number,"phone_number");
		SafeGetString(result,user->email,"email");
		SafeGetString(result,user->avatar,"avatar");
		SafeGetInt(result,user->sex,"sex");
		SafeGetInt(result,user->status,"status");
		SafeGetInt(result,user->is_deleted,"is_deleted");
		SafeGetString(result,user->birthday,"birthday");
		SafeGetString(result,user->login_time,"login_time");
		SafeGetString(result,user->create_time,"create_time");
		SafeGetString(result,user->create_by,"create_by");
		SafeGetString(result,user->update_time,"update_time");
		SafeGetString(result,user->update_by,"update_by");
		SafeGetString(result,user->remark,"remark");
		return user;
	}
public:
	Object<SysUser> createUser(const Object<SysUser>& user, const zc::mysql::PooledConnection& con) override
	{
		auto query = "INSERT INTO sys_user(user_name,password,nick_name,dept_id) VALUE(?,?,?,100)";
		try {
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, *user->user_name);
			prepared->setString(2, *user->password);
			prepared->setString(3, *user->nick_name);

			prepared->execute();

			return selectByUsername(*user->user_name, con);
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	bool updatePwd(const Object<SysUser>& user, const zc::mysql::PooledConnection& con) override
	{
		auto query = "UPDATE sys_user SET password = ?,update_time=NOW(),update_by=? WHERE id = ?";
		try {
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, *user->password);
			prepared->setString(2, *user->update_by);
			prepared->setInt(3, *user->user_id);
			return prepared->executeUpdate() > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	bool updateAvatar(const Object<SysUser>& user, const zc::mysql::PooledConnection& con) override
	{
		auto query = "UPDATE sys_user SET avatar = ?,update_time=NOW() WHERE id = ?";
		try {
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, *user->avatar);
			prepared->setInt(2, *user->user_id);
			return prepared->executeUpdate() > 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
public:
	Object<SysUser> selectById(int32_t id, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_user WHERE id = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setInt(1, id);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return toEntity(result.get());
			}
			return nullptr;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	bool insert(const Object<SysUser>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Insert("dept_id"_c, "user_name"_c, "nick_name"_c, "password"_c, "email"_c,
			"phone_number"_c, "sex"_c, "status"_c,"create_time"_c, "create_by"_c, "remark"_c)
			.values(e->dept_id,e->user_name,e->nick_name,e->password,e->email,
				e->phone_number,e->sex,e->status,now(),e->create_by,e->remark)
			.into("sys_user")
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
	int32_t update(const Object<SysUser>& e, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Update("sys_user")
			.set("dept_id"_c = e->dept_id)
			    ("nick_name"_c = e->nick_name)
			    //("user_name"_c = e->user_name)
			    ("email"_c = e->email)
			    ("phone_number"_c = e->phone_number)
			    ("sex"_c = e->sex)
			    ("status"_c = e->status)
			    ("update_by"_c = e->update_by)
			    ("update_time"_c = zc::tool::sql::now())
			    ("remark"_c = e->remark)
			    ("login_time"_c = e->login_time)
			.where("id"_c == e->user_id)
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

		//?,?,?,?,? 1,2,3,4,5
		std::string  placeholders;
		for (size_t i = 0; i < ids.size(); i++) {
			placeholders += '?';
			if (i < ids.size() - 1) {
				placeholders += ",";
			}
		}

		auto query = "UPDATE sys_user SET is_deleted = 1 WHERE id IN(" + placeholders + ")";

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
	Object<SysUser> selectByUsername(const std::string& username, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_user WHERE user_name = ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, username);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return toEntity(result.get());
			}
			return nullptr;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	Object<SysUser> selectByPhone(const std::string& phone, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_user WHERE phone_number= ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, phone);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return toEntity(result.get());
			}
			return nullptr;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	Object<SysUser> selectByEmail(const std::string& email, const zc::mysql::PooledConnection& con) override
	{
		auto query = "SELECT * FROM sys_user WHERE email= ?";
		try
		{
			std::unique_ptr<sql::PreparedStatement> prepared(con->prepareStatement(query));
			prepared->setString(1, email);

			std::unique_ptr<sql::ResultSet> result(prepared->executeQuery());
			if (result->next()) {
				return toEntity(result.get());
			}
			return nullptr;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
public:
	std::pair<ObjectList<SysUser>, int32_t>selectUserList(const Object<SysUser>& user, int32_t page, int32_t pageSize, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Select(all)
			.from("sys_user")
			.where("user_name"_c.like(user->user_name) and "nick_name"_c.like(user->nick_name) 
				and "phone_number"_c.like(user->phone_number) and "sex"_c == user->sex and "status"_c == user->status
				and "is_deleted"_c == false
				and "create_time"_c.between_and(user->params.begTime, user->params.endTime))
			.order_by("id"_c.desc())
			.limit((page - 1) * pageSize, pageSize)
			.sql();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());

			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			ObjectList<SysUser> list;
			while(result->next()) {
				list.push_back(std::move(toEntity(result.get())));
			}
			return { list, selectCount(query,con) };
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	static int32_t selectCount(const std::string& sql,const zc::mysql::PooledConnection& con)
	{
		auto query = std::format("SELECT COUNT(*) FROM ({}) t", sql);
		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());

			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			if (result->next()) {
				return result->getInt(1);
			}
			return 0;
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}

	std::pair<ObjectList<SysUser>, int32_t> selectAllocatedList(const Object<SysUser>& user, int32_t page, int32_t pageSize, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto query = Select("DISTINCT u.id"_c, "u.user_name"_c, "u.nick_name"_c, "u.email"_c, "u.phone_number"_c, "u.status"_c, "u.create_time"_c)
			.from("sys_user").as("u")
			.left_join("sys_user_role").as("ur")
			.on("u.id"_c == "ur.user_id"_c)
			.where("u.is_deleted"_c == false and "ur.role_id"_c == user->role_id
				and "u.user_name"_c.like(user->user_name) and "u.phone_number"_c.like(user->phone_number))
			.order_by("u.id"_c.desc())
			.limit((page - 1) * pageSize, pageSize)
			.sql();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());

			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			ObjectList<SysUser> list;
			while (result->next()) {
				list.push_back(std::move(toEntity(result.get())));
			}
			return { list ,selectCount(query,con) };
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
	std::pair<ObjectList<SysUser>, int32_t>  selectUnallocatedList(const Object<SysUser>& user, int32_t page, int32_t pageSize, const zc::mysql::PooledConnection& con) override
	{
		using namespace zc::tool::sql;
		using namespace zc::tool::sql::literals;

		auto subQuery = Select("u.id"_c)
			.from("sys_user").as("u")
			.join("sys_user_role").as("ur")
			.on("u.id"_c == "ur.user_id")
			.where("ur.role_id"_c == user->role_id);

		auto query = Select("DISTINCT ur.role_id"_c, "u.id"_c, "u.user_name"_c, "u.nick_name"_c, "u.email"_c, "u.phone_number"_c, "u.status"_c, "u.create_time"_c)
			.from("sys_user").as("u")
			.left_join("sys_user_role").as("ur")
			.on("u.id"_c == "ur.user_id"_c)
			.where("u.is_deleted"_c == false and ("ur.role_id"_c != user->role_id or "ur.role_id"_c.is_null())
				and "u.id"_c.not_in(subQuery) and "u.user_name"_c.like(user->user_name) and
				"u.phone_number"_c.like(user->phone_number))
			.order_by("u.id"_c.desc())
			.limit((page - 1) * pageSize, pageSize)
			.sql();

		try
		{
			std::unique_ptr<sql::Statement> stmt(con->createStatement());

			std::unique_ptr<sql::ResultSet> result(stmt->executeQuery(query));
			ObjectList<SysUser> list;
			while (result->next()) {
				list.push_back(std::move(toEntity(result.get())));
			}
			return { list,selectCount(query,con) };
		}
		catch (const sql::SQLException& e) {
			handleException(e, query);
		}
	}
};


std::shared_ptr<UserDao> UserDao::createShared()
{
	return std::make_shared<MysqlUserDao>();
}

