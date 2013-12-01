#include "PolyVoxCore/Impl/Timer.h"
#include "PolyVoxCore/Impl/Utility.h"

#include "SQLiteUtils.h"

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
	VoxelDatabase<VoxelType>::VoxelDatabase(sqlite3* database)
		:Pager<VoxelType>()
		,mDatabase(database)
		,m_pCompressor(0)
	{
		// Create the 'Blocks' table if it doesn't exist. Not sure we need 'ASC'
		// here, but it's in the example (http://goo.gl/NLHjQv) as is the default anyway.
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "CREATE TABLE IF NOT EXISTS Blocks(Region INTEGER PRIMARY KEY ASC, Data BLOB);", 0, 0, 0) );

		// Now create the 'OverrideBlocks' table if it doesn't exist. Not sure we need 'ASC'
		// here, but it's in the example (http://goo.gl/NLHjQv) as is the default anyway.
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "CREATE TABLE IF NOT EXISTS OverrideBlocks(Region INTEGER PRIMARY KEY ASC, Data BLOB);", 0, 0, 0) );

		// Now build the 'insert or replace' prepared statements
		EXECUTE_SQLITE_FUNC( sqlite3_prepare_v2(mDatabase, "INSERT OR REPLACE INTO Blocks (Region, Data) VALUES (?, ?)", -1, &mInsertOrReplaceBlockStatement, NULL) );
		EXECUTE_SQLITE_FUNC( sqlite3_prepare_v2(mDatabase, "INSERT OR REPLACE INTO OverrideBlocks (Region, Data) VALUES (?, ?)", -1, &mInsertOrReplaceOverrideBlockStatement, NULL) );

		// Now build the 'select' prepared statements
		EXECUTE_SQLITE_FUNC( sqlite3_prepare_v2(mDatabase, "SELECT Data FROM Blocks WHERE Region = ?", -1, &mSelectBlockStatement, NULL) );
		EXECUTE_SQLITE_FUNC( sqlite3_prepare_v2(mDatabase, "SELECT Data FROM OverrideBlocks WHERE Region = ?", -1, &mSelectOverrideBlockStatement, NULL) );

		//Create the compressor
		m_pCompressor = new ::PolyVox::MinizBlockCompressor<VoxelType>;
	}

	/// Destructor
	template <typename VoxelType>
	VoxelDatabase<VoxelType>::~VoxelDatabase()
	{
		finalizeStatementWithLogging(mSelectBlockStatement);
		finalizeStatementWithLogging(mSelectOverrideBlockStatement);
		finalizeStatementWithLogging(mInsertOrReplaceBlockStatement);
		finalizeStatementWithLogging(mInsertOrReplaceOverrideBlockStatement);

		delete m_pCompressor;
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::compress(PolyVox::UncompressedBlock<VoxelType>* pSrcBlock, PolyVox::CompressedBlock<VoxelType>* pDstBlock)
	{
		// Pass through to compressor
		m_pCompressor->compress(pSrcBlock, pDstBlock);
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::decompress(PolyVox::CompressedBlock<VoxelType>* pSrcBlock, PolyVox::UncompressedBlock<VoxelType>* pDstBlock)
	{
		// Pass through to compressor
		m_pCompressor->decompress(pSrcBlock, pDstBlock);
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::pageIn(const PolyVox::Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
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
			pBlockData->setData(static_cast<const uint8_t*>(data), length);
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
				pBlockData->setData(static_cast<const uint8_t*>(data), length);
			}
		}

		logTrace() << "Paged block in in " << timer.elapsedTimeInMilliSeconds() << "ms";
	}

	template <typename VoxelType>
	void VoxelDatabase<VoxelType>::pageOut(const PolyVox::Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData)
	{
		POLYVOX_ASSERT(pBlockData, "Attempting to page out NULL block");

		PolyVox::Timer timer;

		logTrace() << "Paging out data for " << region;

		int64_t key = regionToKey(region);

		// Based on: http://stackoverflow.com/a/5308188
		sqlite3_reset(mInsertOrReplaceOverrideBlockStatement);
		sqlite3_bind_int64(mInsertOrReplaceOverrideBlockStatement, 1, key);
		sqlite3_bind_blob(mInsertOrReplaceOverrideBlockStatement, 2, static_cast<const void*>(pBlockData->getData()), pBlockData->getDataSizeInBytes(), SQLITE_TRANSIENT);
		sqlite3_step(mInsertOrReplaceOverrideBlockStatement);

		logTrace() << "Paged block out in " << timer.elapsedTimeInMilliSeconds() << "ms (" << pBlockData->getDataSizeInBytes() << "bytes of data)";
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
}
