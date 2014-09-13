#include "PolyVoxCore/Impl/Timer.h"
#include "PolyVoxCore/Impl/Utility.h"

#include "SQLiteUtils.h"

#include <climits>

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
	VoxelDatabase<VoxelType>::VoxelDatabase()
		:PolyVox::Pager<VoxelType>()
	{
	}

	/// Destructor
	template <typename VoxelType>
	VoxelDatabase<VoxelType>::~VoxelDatabase()
	{
		EXECUTE_SQLITE_FUNC( sqlite3_finalize(mSelectBlockStatement) );
		EXECUTE_SQLITE_FUNC( sqlite3_finalize(mSelectOverrideBlockStatement) );
		EXECUTE_SQLITE_FUNC( sqlite3_finalize(mInsertOrReplaceBlockStatement) );
		EXECUTE_SQLITE_FUNC( sqlite3_finalize(mInsertOrReplaceOverrideBlockStatement) );
		EXECUTE_SQLITE_FUNC( sqlite3_finalize(mSelectPropertyStatement) );
		EXECUTE_SQLITE_FUNC( sqlite3_finalize(mInsertOrReplacePropertyStatement) );

		if (sqlite3_db_readonly(mDatabase, "main") == 0)
		{
			POLYVOX_LOG_TRACE("Vacuuming database...");
			try
			{
				PolyVox::Timer timer;
				EXECUTE_SQLITE_FUNC(sqlite3_exec(mDatabase, "VACUUM;", 0, 0, 0));
				POLYVOX_LOG_TRACE("Vacuumed database in " << timer.elapsedTimeInMilliSeconds() << "ms");
			}
			catch (SQLiteError& e)
			{
				// It seems that vacuuming of the database can fail even when opened in readwrite mode, if other processes are still
				// accessing the database. This can happen if multiple volumes are sharing the database. This shouldn't really matter
				// as the database will probably get vacuumed at some point in the future, and it's not essential anyway.
				POLYVOX_LOG_WARNING("Failed to vacuum database. Error message was as follows:" << std::endl << "\t" << e.what());
			}
		}

		EXECUTE_SQLITE_FUNC(sqlite3_close(mDatabase));
	}

	template <typename VoxelType>
	VoxelDatabase<VoxelType>* VoxelDatabase<VoxelType>::createEmpty(const std::string& pathToNewVoxelDatabase)
	{
		// Make sure that the provided path doesn't already exist.
		// If the file is NULL then we don't need to (and can't) close it.
		FILE* file = fopen(pathToNewVoxelDatabase.c_str(), "rb");
		if (file != NULL)
		{
			fclose(file);
			POLYVOX_THROW(std::invalid_argument, "Cannot create a new voxel database as the provided filename (" << pathToNewVoxelDatabase << ") already exists");
		}

		POLYVOX_LOG_INFO("Creating empty voxel database as '" << pathToNewVoxelDatabase << "'");
		VoxelDatabase<VoxelType>* voxelDatabase = new VoxelDatabase<VoxelType>;
		EXECUTE_SQLITE_FUNC(sqlite3_open_v2(pathToNewVoxelDatabase.c_str(), &(voxelDatabase->mDatabase), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL));

		// Create the 'Properties' table.
		EXECUTE_SQLITE_FUNC(sqlite3_exec(voxelDatabase->mDatabase, "CREATE TABLE Properties(Name TEXT PRIMARY KEY, Value TEXT);", 0, 0, 0));

		// Create the 'Blocks' table. Not sure we need 'ASC' here, but it's in the example (http://goo.gl/NLHjQv) as is the default anyway.
		EXECUTE_SQLITE_FUNC(sqlite3_exec(voxelDatabase->mDatabase, "CREATE TABLE Blocks(Region INTEGER PRIMARY KEY ASC, Data BLOB);", 0, 0, 0));

		voxelDatabase->initialize();
		return voxelDatabase;
	}

	template <typename VoxelType>
	VoxelDatabase<VoxelType>* VoxelDatabase<VoxelType>::createFromVDB(const std::string& pathToExistingVoxelDatabase, WritePermission writePermission)
	{
		// When creating a new empty voxel database the user can pass an empty string to signify that 
		// the database will be temporary, but when creating from a VDB a valid path must be provided.
		POLYVOX_THROW_IF(pathToExistingVoxelDatabase.empty(), std::invalid_argument, "Path must not be an empty string");

		POLYVOX_LOG_INFO("Creating voxel database from '" << pathToExistingVoxelDatabase << "'");
		VoxelDatabase<VoxelType>* voxelDatabase = new VoxelDatabase<VoxelType>;
		int flags = (writePermission == WritePermissions::ReadOnly) ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE;
		EXECUTE_SQLITE_FUNC(sqlite3_open_v2(pathToExistingVoxelDatabase.c_str(), &(voxelDatabase->mDatabase), flags, NULL));

		voxelDatabase->initialize();
		return voxelDatabase;
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::initialize(void)
	{
		// Disable syncing
		EXECUTE_SQLITE_FUNC(sqlite3_exec(mDatabase, "PRAGMA synchronous = OFF", 0, 0, 0));

		// Now create the 'OverrideBlocks' table. Not sure we need 'ASC' here, but it's in the example (http://goo.gl/NLHjQv) and is the default anyway.
		// Note that the table cannot already exist because it's created as 'TEMP', and is therefore stored in a seperate temporary database.
		// It appears this temporary table is not shared between connections (multiple volumes using the same VDB) which is probably desirable for us
		// as it means different instances of the volume can be modified (but not commited to) without interfering with each other (http://goo.gl/aDKyId).
		EXECUTE_SQLITE_FUNC(sqlite3_exec(mDatabase, "CREATE TEMP TABLE OverrideBlocks(Region INTEGER PRIMARY KEY ASC, Data BLOB);", 0, 0, 0));

		// Now build the 'insert or replace' prepared statements
		EXECUTE_SQLITE_FUNC(sqlite3_prepare_v2(mDatabase, "INSERT OR REPLACE INTO Blocks (Region, Data) VALUES (?, ?)", -1, &mInsertOrReplaceBlockStatement, NULL));
		EXECUTE_SQLITE_FUNC(sqlite3_prepare_v2(mDatabase, "INSERT OR REPLACE INTO OverrideBlocks (Region, Data) VALUES (?, ?)", -1, &mInsertOrReplaceOverrideBlockStatement, NULL));

		// Now build the 'select' prepared statements
		EXECUTE_SQLITE_FUNC(sqlite3_prepare_v2(mDatabase, "SELECT Data FROM Blocks WHERE Region = ?", -1, &mSelectBlockStatement, NULL));
		EXECUTE_SQLITE_FUNC(sqlite3_prepare_v2(mDatabase, "SELECT Data FROM OverrideBlocks WHERE Region = ?", -1, &mSelectOverrideBlockStatement, NULL));

		// Now build the 'select' and 'insert or replace' prepared statements
		EXECUTE_SQLITE_FUNC(sqlite3_prepare_v2(mDatabase, "SELECT Value FROM Properties WHERE Name = ?", -1, &mSelectPropertyStatement, NULL));
		EXECUTE_SQLITE_FUNC(sqlite3_prepare_v2(mDatabase, "INSERT OR REPLACE INTO Properties (Name, Value) VALUES (?, ?)", -1, &mInsertOrReplacePropertyStatement, NULL));
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::pageIn(const PolyVox::Region& region, PolyVox::UncompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page in NULL block");

		PolyVox::Timer timer;

		int64_t key = regionToKey(region);

		// First we try and read the data from the OverrideBlocks table
		// Based on: http://stackoverflow.com/a/5308188
		sqlite3_reset(mSelectOverrideBlockStatement);
		sqlite3_bind_int64(mSelectOverrideBlockStatement, 1, key);
		if(sqlite3_step(mSelectOverrideBlockStatement) == SQLITE_ROW)
        {
			// I think the last index is zero because our select statement only returned one column.
            int length = sqlite3_column_bytes(mSelectOverrideBlockStatement, 0);
            const void* data = sqlite3_column_blob(mSelectOverrideBlockStatement, 0);

			//m_pCompressor->decompressWithMiniz(data, length, pBlockData->getData(), pBlockData->getDataSizeInBytes());

			mz_ulong uncomp_len;
			int cmp_status = uncompress((unsigned char*)pBlockData->getData(), &uncomp_len, (const unsigned char*)data, length);

			//pBlockData->setData(static_cast<const uint8_t*>(data), length);
        }
		else
		{
			// In this case the block data wasn't found in the override table, so we go to the real Blocks table.
			sqlite3_reset(mSelectBlockStatement);
			sqlite3_bind_int64(mSelectBlockStatement, 1, key);
			if(sqlite3_step(mSelectBlockStatement) == SQLITE_ROW)
			{
				// I think the last index is zero because our select statement only returned one column.
				int length = sqlite3_column_bytes(mSelectBlockStatement, 0);
				const void* data = sqlite3_column_blob(mSelectBlockStatement, 0);

				//m_pCompressor->decompressWithMiniz(data, length, pBlockData->getData(), pBlockData->getDataSizeInBytes());

				mz_ulong uncomp_len;
				int cmp_status = uncompress((unsigned char*)pBlockData->getData(), &uncomp_len, (const unsigned char*)data, length);

				//pBlockData->setData(static_cast<const uint8_t*>(data), length);
			}
		}

		POLYVOX_LOG_TRACE("Paged block in in " << timer.elapsedTimeInMilliSeconds() << "ms");
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::pageOut(const PolyVox::Region& region, PolyVox::UncompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");

		PolyVox::Timer timer;

		POLYVOX_LOG_TRACE("Paging out data for " << region);

		//const size_t dstLength = 1000000; //HACK!!!
		//uint8_t* dstBuffer = new uint8_t[dstLength];

		//uint32_t resultLength = m_pCompressor->compressWithMiniz(pBlockData->getData(), pBlockData->getDataSizeInBytes(), dstBuffer, dstLength);

		uLong src_len = pBlockData->getDataSizeInBytes();
		uLong cmp_len = compressBound(src_len);

		uint8_t* pCmp = new uint8_t[cmp_len];

		int cmp_status = compress(pCmp, &cmp_len, (const unsigned char *)pBlockData->getData(), src_len);

		int64_t key = regionToKey(region);

		// Based on: http://stackoverflow.com/a/5308188
		sqlite3_reset(mInsertOrReplaceOverrideBlockStatement);
		sqlite3_bind_int64(mInsertOrReplaceOverrideBlockStatement, 1, key);
		sqlite3_bind_blob(mInsertOrReplaceOverrideBlockStatement, 2, static_cast<const void*>(pCmp), cmp_len, SQLITE_TRANSIENT);
		sqlite3_step(mInsertOrReplaceOverrideBlockStatement);

		delete[] pCmp;

		POLYVOX_LOG_TRACE("Paged block out in " << timer.elapsedTimeInMilliSeconds() << "ms (" << pBlockData->getDataSizeInBytes() << "bytes of data)");
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::acceptOverrideBlocks(void)
	{
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "INSERT OR REPLACE INTO Blocks (Region, Data) SELECT Region, Data from OverrideBlocks;", 0, 0, 0) );

		// The override blocks have been copied accross so we
		// can now discard the contents of the override table.
		discardOverrideBlocks();
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::discardOverrideBlocks(void)
	{
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "DELETE FROM OverrideBlocks;", 0, 0, 0) );
	}

	template <typename VoxelType>
	bool VoxelDatabase<VoxelType>::getProperty(const std::string& name, std::string& value)
	{
		EXECUTE_SQLITE_FUNC(sqlite3_reset(mSelectPropertyStatement));
		EXECUTE_SQLITE_FUNC(sqlite3_bind_text(mSelectPropertyStatement, 1, name.c_str(), -1, SQLITE_TRANSIENT));
		if (sqlite3_step(mSelectPropertyStatement) == SQLITE_ROW)
		{
			// I think the last index is zero because our select statement only returned one column.
			value = std::string(reinterpret_cast<const char*>(sqlite3_column_text(mSelectPropertyStatement, 0)));
			return true;
		}
		else
		{
			POLYVOX_LOG_WARNING("Property '" << name << "' was not found. The default value will be used instead");
			return false;
		}
	}

	template <typename VoxelType>
	int32_t VoxelDatabase<VoxelType>::getPropertyAsInt(const std::string& name, int32_t defaultValue)
	{
		std::string value;
		if (getProperty(name, value))
		{
			return ::atol(value.c_str());
		}

		return defaultValue;
	}

	template <typename VoxelType>
	float VoxelDatabase<VoxelType>::getPropertyAsFloat(const std::string& name, float defaultValue)
	{
		std::string value;
		if (getProperty(name, value))
		{
			return ::atof(value.c_str());
		}

		return defaultValue;
	}

	template <typename VoxelType>
	std::string VoxelDatabase<VoxelType>::getPropertyAsString(const std::string& name, const std::string& defaultValue)
	{
		std::string value;
		if (getProperty(name, value))
		{
			return value;
		}

		return defaultValue;
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::setProperty(const std::string& name, int value)
	{
		std::stringstream ss;
		ss << value;
		setProperty(name, ss.str());
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::setProperty(const std::string& name, float value)
	{
		std::stringstream ss;
		ss << value;
		setProperty(name, ss.str());
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::setProperty(const std::string& name, const std::string& value)
	{
		// Based on: http://stackoverflow.com/a/5308188
		EXECUTE_SQLITE_FUNC(sqlite3_reset(mInsertOrReplacePropertyStatement));
		EXECUTE_SQLITE_FUNC(sqlite3_bind_text(mInsertOrReplacePropertyStatement, 1, name.c_str(), -1, SQLITE_TRANSIENT));
		EXECUTE_SQLITE_FUNC(sqlite3_bind_text(mInsertOrReplacePropertyStatement, 2, value.c_str(), -1, SQLITE_TRANSIENT));
		sqlite3_step(mInsertOrReplacePropertyStatement); //Don't wrap this one as it isn't supposed to return SQLITE_OK?
	}
}
