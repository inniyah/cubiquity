#ifndef __CUBIQUITY_OVERRIDE_FILE_PAGER_H__
#define __CUBIQUITY_OVERRIDE_FILE_PAGER_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include "PolyVoxCore/Pager.h"
#include "PolyVoxCore/Region.h"

#include <stdexcept>
#include <string>

namespace Cubiquity
{
	/**
	 * Provides an interface for performing paging of data.
	 */
	template <typename VoxelType>
	class OverrideFilePager : public PolyVox::Pager<VoxelType>
	{
	public:
		/// Constructor
		OverrideFilePager(const std::string& strFolderName);

		/// Destructor
		virtual ~OverrideFilePager();

		virtual void pageIn(const Region& region, PolyVox::Block<VoxelType>* pBlockData);
		virtual void pageOut(const Region& region, PolyVox::Block<VoxelType>* pBlockData);

	protected:
		std::string m_strFolderName;
		std::string m_strOverrideFolderName;
	};
}

#include "OverrideFilePager.inl"

#endif //__CUBIQUITY_OVERRIDE_FILE_PAGER_H__
