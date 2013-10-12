#ifndef __CUBIQUITY_SQLITE_PAGER_H__
#define __CUBIQUITY_SQLITE_PAGER_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include "PolyVoxCore/Pager.h"
#include "PolyVoxCore/Region.h"

#include "SQLite/sqlite3.h"

namespace Cubiquity
{
	/**
	 * Provides an interface for performing paging of data.
	 */
	template <typename VoxelType>
	class SQLitePager : public PolyVox::Pager<VoxelType>
	{
	public:
		/// Constructor
		SQLitePager(const std::string& dbName);

		/// Destructor
		virtual ~SQLitePager();

		virtual void pageIn(const Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData);
		virtual void pageOut(const Region& region, PolyVox::CompressedBlock<VoxelType>* pBlockData);

	private:

		sqlite3* pDatabase;
		sqlite3_stmt* pReplaceBlockStatement;
		sqlite3_stmt* pSelectBlockStatement;
	};

	// From http://stackoverflow.com/a/776550
	// Should only be used on unsigned types.
	template <typename T> 
	T rol(T val)
	{
		return (val << 1) | (val >> (sizeof(T)*CHAR_BIT-1));
	}
}

#include "SQLitePager.inl"

#endif //__CUBIQUITY_SQLITE_PAGER_H__