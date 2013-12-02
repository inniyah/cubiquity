#ifndef __CUBIQUITY_SQLITE_PAGER_H__
#define __CUBIQUITY_SQLITE_PAGER_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include "PolyVoxCore/MinizBlockCompressor.h"
#include "PolyVoxCore/Pager.h"
#include "PolyVoxCore/Region.h"

#include "SQLite/sqlite3.h"

namespace Cubiquity
{
	/**
	 * Provides an interface for performing paging of data.
	 */
	template <typename VoxelType>
	class VoxelDatabase : public PolyVox::BlockCompressor<VoxelType>, public PolyVox::Pager<VoxelType>
	{
	public:
		/// Constructor
		VoxelDatabase(sqlite3* database);

		/// Destructor
		virtual ~VoxelDatabase();

		virtual void compress(PolyVox::UncompressedBlock<VoxelType>* pSrcBlock, PolyVox::CompressedBlock<VoxelType>* pDstBlock);
		virtual void decompress(PolyVox::CompressedBlock<VoxelType>* pSrcBlock, PolyVox::UncompressedBlock<VoxelType>* pDstBlock);

		virtual void pageIn(const PolyVox::Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData);
		virtual void pageOut(const PolyVox::Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData);

		void acceptOverrideBlocks(void);
		void discardOverrideBlocks(void);

	private:

		sqlite3* mDatabase;

		sqlite3_stmt* mSelectBlockStatement;
		sqlite3_stmt* mSelectOverrideBlockStatement;
		
		sqlite3_stmt* mInsertOrReplaceBlockStatement;
		sqlite3_stmt* mInsertOrReplaceOverrideBlockStatement;

		::PolyVox::MinizBlockCompressor<VoxelType>* m_pCompressor;
	};

	// Utility function to perform bit rotation.
	template <typename T> 
	T rotateLeft(T val);

	// Allows us to use a Region as a key in the SQLite database.
	uint64_t regionToKey(const PolyVox::Region& region);
}

#include "VoxelDatabase.inl"

#endif //__CUBIQUITY_SQLITE_PAGER_H__