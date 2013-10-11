#ifndef __CUBIQUITY_SQLITE_PAGER_H__
#define __CUBIQUITY_SQLITE_PAGER_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include "PolyVoxCore/Pager.h"
#include "PolyVoxCore/Region.h"

namespace Cubiquity
{
	template <typename VoxelType>
	class SQLitePager : public PolyVox::Pager<VoxelType>
	{
	public:
	};
}

#include "SQLitePager.inl"

#endif //__CUBIQUITY_SQLITE_PAGER_H__