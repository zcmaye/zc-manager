#include "ConnectionPool.h"
#include "common/exception/BusinessError.hpp"

class MysqlUtils
{
public:
	inline static int32_t lastInsertId(const zc::mysql::PooledConnection& con)
	{
		auto query = "SELECT LAST_INSERT_ID()";
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
			throw BusinessError(std::format("Error {}({}) : {}", e.getErrorCode(), e.getSQLState(), e.what()));
		}
	}
};

