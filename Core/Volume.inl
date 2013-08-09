#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/MinizBlockCompressor.h"
#include "PolyVoxCore/Raycast.h"
#include "PolyVoxCore/VolumeResampler.h"

#include "PolyVoxCore/Impl/Utility.h" //Should we include from Impl?

#include "Clock.h"
#include "BackgroundTaskProcessor.h"
#include "Logging.h"
#include "MainThreadTaskProcessor.h"
#include "MultiMaterial.h"
#include "OverrideFilePager.h"
#include "Raycasting.h"

#include <boost/filesystem.hpp>

#include <stdlib.h>
#include <time.h>

namespace Cubiquity
{
	template <typename VoxelType>
	Volume<VoxelType>::Volume(const Region& region, const std::string& pageFolder, OctreeConstructionMode octreeConstructionMode, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
#ifdef USE_LARGE_VOLUME
		,m_pCompressor(0)
		,m_pOverrideFilePager(0)
#endif
		,mOctree(0)
	{
		logTrace() << "Entering Volume(" << region << ",...)";

		/*POLYVOX_ASSERT(region.getWidthInVoxels() > 0, "All volume dimensions must be greater than zero");
		POLYVOX_ASSERT(region.getHeightInVoxels() > 0, "All volume dimensions must be greater than zero");
		POLYVOX_ASSERT(region.getDepthInVoxels() > 0, "All volume dimensions must be greater than zero");*/
		POLYVOX_THROW_IF(region.getWidthInVoxels() == 0, std::invalid_argument, "Volume width must be greater than zero");
		POLYVOX_THROW_IF(region.getHeightInVoxels() == 0, std::invalid_argument, "Volume height must be greater than zero");
		POLYVOX_THROW_IF(region.getDepthInVoxels() == 0, std::invalid_argument, "Volume depth must be greater than zero");
	
#ifdef USE_LARGE_VOLUME
		m_pCompressor = new ::PolyVox::MinizBlockCompressor<VoxelType>;

		if(pageFolder.size() != 0)
		{
			m_pOverrideFilePager = new OverrideFilePager<VoxelType>(pageFolder);
		}
		else
		{
			// Note: The code below crashes in gameplay when trying to use Boost filesystem. It is
			// probably related to this issue: https://github.com/blackberry/GamePlay/issues/919
			// For no we just use the current folder instead.
			// Create a random folder name
			/*srand(static_cast<unsigned int>(time(NULL)));
			std::stringstream ss;
			ss << "./page_data_" << rand() << "/";
			logInfo() << "No page folder was provided, using '" << ss.str() << "' as temporary storage";

			// Create the folder
			boost::filesystem::path dir(ss.str());
			if (boost::filesystem::create_directory(dir))
			{
				m_pFilePager = new FilePager<VoxelType>(ss.str());
			}
			else
			{
				POLYVOX_THROW(std::runtime_error, "Failed to create temporary folder for page data");
			}*/

			logInfo() << "No page folder was provided, using current working directory as temporary storage";
			m_pOverrideFilePager = new OverrideFilePager<VoxelType>("./");
		}
		
		//FIXME - This should be decided based on the Octree type but instead be in diffferent volume constructors
		if(octreeConstructionMode == OctreeConstructionModes::BoundCells) // Smooth terrain
		{
			mPolyVoxVolume = new ::PolyVox::POLYVOX_VOLUME<VoxelType>(region, m_pCompressor, m_pOverrideFilePager, 32);
		}
		else // Cubic terrain
		{
			mPolyVoxVolume = new ::PolyVox::POLYVOX_VOLUME<VoxelType>(region, m_pCompressor, m_pOverrideFilePager, 64);
		}

		mPolyVoxVolume->setMaxNumberOfBlocksInMemory(100000000);
		mPolyVoxVolume->setMaxNumberOfUncompressedBlocks(1000000);

#else
		//FIXME - This should be decided based on the Octree type but instead be in diffferent volume constructors
		if(octreeConstructionMode == OctreeConstructionModes::BoundCells) // Smooth terrain
		{
			mPolyVoxVolume = new ::PolyVox::POLYVOX_VOLUME<VoxelType>(region, 32);
		}
		else // Cubic terrain
		{
			mPolyVoxVolume = new ::PolyVox::POLYVOX_VOLUME<VoxelType>(region, 64);
		}
#endif

		mOctree = new Octree<VoxelType>(this, octreeConstructionMode, baseNodeSize);

		logTrace() << "Leaving Volume(" << region << ",...)";
	}

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		logTrace() << "Entering ~Volume()";

		// NOTE: We should really delete the volume here, but the background task processor might still be using it.
		// We need a way to shut that down, or maybe smart pointers can help here. Just flush until we have a better fix.
#ifdef USE_LARGE_VOLUME
		mPolyVoxVolume->flushAll();
#endif

		logTrace() << "Exiting ~Volume()";
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(int32_t x, int32_t y, int32_t z) const
	{
		// Border value is returned for invalid position
		return mPolyVoxVolume->getVoxel<::PolyVox::WrapModes::Border>(x, y, z, VoxelType());
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(int32_t x, int32_t y, int32_t z, VoxelType value, UpdatePriority updatePriority)
	{
		// Validate the voxel position
		POLYVOX_THROW_IF(mPolyVoxVolume->getEnclosingRegion().containsPoint(x, y, z) == false,
			std::invalid_argument, "Attempted to write to a voxel which is outside of the volume");

		mPolyVoxVolume->setVoxelAt(x, y, z, value);
		if(updatePriority != UpdatePriorities::DontUpdate)
		{
			mOctree->markDataAsModified(x, y, z, Clock::getTimestamp(), updatePriority);
		}
	}

	template <typename VoxelType>
	void Volume<VoxelType>::markAsModified(const Region& region, UpdatePriority updatePriority)
	{
		POLYVOX_THROW_IF(updatePriority == UpdatePriorities::DontUpdate, std::invalid_argument, "You cannot mark as modified yet request no update");

		mOctree->markDataAsModified(region, Clock::getTimestamp(), updatePriority);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::update(const Vector3F& viewPosition, float lodThreshold)
	{
		mOctree->update(viewPosition, lodThreshold);
	}
}
