#ifndef CUBIQUITY_SQLITEUTILS_H_
#define CUBIQUITY_SQLITEUTILS_H_

#include "Exceptions.h"

#include "SQLite/sqlite3.h"

#include "PolyVox/Impl/ErrorHandling.h"

#include <stdexcept>
#include <sstream>

namespace Cubiquity
{
	#define EXECUTE_SQLITE_FUNC(function) \
		do \
		{ \
			int rc = function; \
			if(rc != SQLITE_OK) \
			{ \
				POLYVOX_THROW(DatabaseError, "Encountered '" << sqlite3_errstr(rc) << "' (error code " << rc << ") when executing '" << #function << "'"); \
			} \
		} while(0)
}

#endif //CUBIQUITY_SQLITEUTILS_H_