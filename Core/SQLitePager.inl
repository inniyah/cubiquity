#include "PolyVoxCore/Impl/Utility.h"

namespace Cubiquity
{
	/// Constructor
	template <typename VoxelType>
	SQLitePager<VoxelType>::SQLitePager(const std::string& dbName)
		:Pager<VoxelType>()
	{
		logDebug() << "Creating SQLitePager from '" << dbName << "'";
		
		int rc = sqlite3_open(dbName.c_str(), &pDatabase);
		if(rc != SQLITE_OK)
		{
			std::stringstream ss;
			ss << "Failed to open SQLite database. Message was: \"" << sqlite3_errmsg(pDatabase) << "\"";
			throw std::runtime_error(ss.str().c_str());
		}

		char* sql = "CREATE TABLE IF NOT EXISTS Blocks(Region INTEGER PRIMARY KEY ASC, Data BLOB);";

		char* pErrorMsg = 0;
		rc = sqlite3_exec(pDatabase, sql, 0, 0, &pErrorMsg);
		if(rc != SQLITE_OK)
		{
			std::stringstream ss;
			ss << "Failed to open SQLite database. Message was: \"" << pErrorMsg << "\"";
			sqlite3_free(pErrorMsg);
			throw std::runtime_error(ss.str().c_str());
		}

		// Now build the prepared statements
		const char* insertQuery = "REPLACE INTO Blocks (Region, Data) VALUES (?, ?)";
		rc = sqlite3_prepare_v2(pDatabase, insertQuery, -1, &pReplaceBlockStatement, NULL);
		if(rc != SQLITE_OK)
		{
			throw std::runtime_error("Failed to prepare insert statement");
		}

		const char* selectQuery = "SELECT Data FROM Blocks WHERE Region = ?";
		rc = sqlite3_prepare_v2(pDatabase, selectQuery, -1, &pSelectBlockStatement, NULL);
		if(rc != SQLITE_OK)
		{
			std::stringstream ss;
			ss << "Failed to prepare select statement. Message was: \"" << sqlite3_errmsg(pDatabase) << "\"";
			throw std::runtime_error(ss.str().c_str());
		}

	}

	/// Destructor
	template <typename VoxelType>
	SQLitePager<VoxelType>::~SQLitePager()
	{
		sqlite3_close(pDatabase);
	}

	template <typename VoxelType>
	void SQLitePager<VoxelType>::pageIn(const Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page in NULL block");

		std::stringstream ss;
		ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				<< region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

		uint32_t x = static_cast<uint32_t>(region.getLowerX());
		uint32_t y = static_cast<uint32_t>(region.getLowerY());
		uint32_t z = static_cast<uint32_t>(region.getLowerZ());

		x = rol(x);
		y = rol(y);
		z = rol(z);

		uint64_t x64 = x;
		uint64_t y64 = y;
		uint64_t z64 = z;

		x64 = x64 << 42;
		y64 = y64 << 21;

		uint64_t result = x64 ^ y64 ^ z64;

		sqlite3_int64 key = static_cast<sqlite3_int64>(result);
		
		// Based on: http://stackoverflow.com/a/5308188
		sqlite3_reset(pSelectBlockStatement);
		//sqlite3_bind_text(pSelectBlockStatement, 1, ss.str().c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int64(pSelectBlockStatement, 1, key);
		if(sqlite3_step(pSelectBlockStatement) == SQLITE_ROW)
        {
			// Indices are zero because our select statement only returned one column?
            int length = sqlite3_column_bytes(pSelectBlockStatement, 0);
            const void* data = sqlite3_column_blob(pSelectBlockStatement, 0);
			pBlockData->setData(static_cast<const uint8_t*>(data), length);
        }
	}

	template <typename VoxelType>
	void SQLitePager<VoxelType>::pageOut(const Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");

		logTrace() << "Paging out data for " << region;

		std::stringstream ss;
		ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				<< region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

		uint32_t x = static_cast<uint32_t>(region.getLowerX());
		uint32_t y = static_cast<uint32_t>(region.getLowerY());
		uint32_t z = static_cast<uint32_t>(region.getLowerZ());

		x = rol(x);
		y = rol(y);
		z = rol(z);

		uint64_t x64 = x;
		uint64_t y64 = y;
		uint64_t z64 = z;

		x64 = x64 << 42;
		y64 = y64 << 21;

		uint64_t result = x64 ^ y64 ^ z64;

		sqlite3_int64 key = static_cast<sqlite3_int64>(result);

		// Based on: http://stackoverflow.com/a/5308188
		//sqlite3_bind_int(pReplaceBlockStatement, 1, 12);
		sqlite3_reset(pReplaceBlockStatement);
		//sqlite3_bind_text(pReplaceBlockStatement, 1, ss.str().c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int64(pReplaceBlockStatement, 1, key);
		sqlite3_bind_blob(pReplaceBlockStatement, 2, static_cast<const void*>(pBlockData->getData()), pBlockData->getDataSizeInBytes(), SQLITE_TRANSIENT);
		sqlite3_step(pReplaceBlockStatement);
	}
}
