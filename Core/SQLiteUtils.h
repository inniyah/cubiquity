#ifndef CUBIQUITY_SQLITEUTILS_H_
#define CUBIQUITY_SQLITEUTILS_H_

#include "SQLite/sqlite3.h"

#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <stdexcept>
#include <sstream>

namespace Cubiquity
{
	class SQLiteError : public std::runtime_error
	{
	public:
		SQLiteError(const std::string& what_arg)
			:runtime_error(what_arg)
		{
		}
	};

	#define EXECUTE_SQLITE_FUNC(function) \
		do \
		{ \
			int rc = function; \
			if(rc != SQLITE_OK) \
			{ \
				POLYVOX_THROW(SQLiteError, sqlite3_errmsg(mDatabase)); \
			} \
		} while(0)
}

#endif //CUBIQUITY_SQLITEUTILS_H_