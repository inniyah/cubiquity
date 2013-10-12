#include "PolyVoxCore/Impl/Utility.h"

namespace Cubiquity
{
	// From http://stackoverflow.com/a/776550
	// Should only be used on unsigned types.
	template <typename T> 
	T rotateLeft(T val)
	{
		return (val << 1) | (val >> (sizeof(T)*CHAR_BIT-1));
	}

	/// Constructor
	template <typename VoxelType>
	SQLitePager<VoxelType>::SQLitePager(const std::string& dbName)
		:Pager<VoxelType>()
	{
		logInfo() << "Creating SQLitePager from '" << dbName << "'";
		
		// Open the database if it already exists.
		int rc = sqlite3_open_v2(dbName.c_str(), &pDatabase, SQLITE_OPEN_READWRITE, NULL);
		if(rc != SQLITE_OK)
		{
			logInfo() << "Failed to open '" << dbName << "'. Error message was: \"" << sqlite3_errmsg(pDatabase) << "\"";

			// If we failed, then try again but this time allow it to be created.
			logInfo() << "Attempting to create '" << dbName << "'";
			rc = sqlite3_open_v2(dbName.c_str(), &pDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
			if(rc != SQLITE_OK)
			{
				// If we failed to create it as well then we give up
				std::stringstream ss;
				ss << "Failed to create '" << dbName << "'. Error message was: \"" << sqlite3_errmsg(pDatabase) << "\"";
				throw std::runtime_error(ss.str().c_str());
			}
			logInfo() << "Successfully created'" << dbName << "'";
		}
		logInfo() << "Successfully opened'" << dbName << "'";


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
	void SQLitePager<VoxelType>::pageIn(const PolyVox::Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page in NULL block");

		std::stringstream ss;
		ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				<< region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

		int64_t key = regionToKey(region);
		
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
	void SQLitePager<VoxelType>::pageOut(const PolyVox::Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");

		logTrace() << "Paging out data for " << region;

		std::stringstream ss;
		ss << region.getLowerX() << "_" << region.getLowerY() << "_" << region.getLowerZ() << "_"
				<< region.getUpperX() << "_" << region.getUpperY() << "_" << region.getUpperZ();

		int64_t key = regionToKey(region);

		// Based on: http://stackoverflow.com/a/5308188
		//sqlite3_bind_int(pReplaceBlockStatement, 1, 12);
		sqlite3_reset(pReplaceBlockStatement);
		//sqlite3_bind_text(pReplaceBlockStatement, 1, ss.str().c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int64(pReplaceBlockStatement, 1, key);
		sqlite3_bind_blob(pReplaceBlockStatement, 2, static_cast<const void*>(pBlockData->getData()), pBlockData->getDataSizeInBytes(), SQLITE_TRANSIENT);
		sqlite3_step(pReplaceBlockStatement);
	}
}
