#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/MinizCompressor.h"
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
		,m_pCompressor(0)
		,m_pOverrideFilePager(0)
		,mOctree(0)
	{
		logTrace() << "Entering Volume(" << region << ",...)";

		POLYVOX_ASSERT(region.getWidthInVoxels() > 0, "All volume dimensions must be greater than zero");
		POLYVOX_ASSERT(region.getHeightInVoxels() > 0, "All volume dimensions must be greater than zero");
		POLYVOX_ASSERT(region.getDepthInVoxels() > 0, "All volume dimensions must be greater than zero");
	
		m_pCompressor = new MinizCompressor;

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
		if(octreeConstructionMode == OctreeConstructionMode::BoundCells) // Smooth terrain
		{
			mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pCompressor, m_pOverrideFilePager, 32);
		}
		else // Cubic terrain
		{
			mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pCompressor, m_pOverrideFilePager, 64);
		}

		mPolyVoxVolume->setMaxNumberOfBlocksInMemory(256);
		mPolyVoxVolume->setMaxNumberOfUncompressedBlocks(128);

		mOctree = new Octree<VoxelType>(this, octreeConstructionMode, baseNodeSize);

		logTrace() << "Leaving Volume(" << region << ",...)";
	}

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		logTrace() << "Entering ~Volume()";

		// NOTE: We should really delete the volume here, but the background task processor might still be using it.
		// We need a way to shut that down, or maybe smart pointers can help here. Just flush until we have a better fix.
		mPolyVoxVolume->flushAll();

		logTrace() << "Exiting ~Volume()";
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(int32_t x, int32_t y, int32_t z, VoxelType value, UpdatePriority updatePriority)
	{
		if(mPolyVoxVolume->getEnclosingRegion().containsPoint(x, y, z))
		{
			mPolyVoxVolume->setVoxelAt(x, y, z, value);
			if(updatePriority != UpdatePriorities::DontUpdate)
			{
				mOctree->markDataAsModified(x, y, z, Clock::getTimestamp(), updatePriority);
			}
		}
		else
		{
			std::stringstream ss;
			ss << "Attempted to write to voxel (" << x << ", " << y << ", " << z << ") which is outside of volume";
			throw std::out_of_range(ss.str());
		}
	}

	template <typename VoxelType>
	void Volume<VoxelType>::markAsModified(const Region& region, UpdatePriority updatePriority)
	{
		POLYVOX_ASSERT(updatePriority != UpdatePriorities::DontUpdate, "You cannot mark as modified yet request no update");
		mOctree->markDataAsModified(region, Clock::getTimestamp(), updatePriority);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::update(const Vector3F& viewPosition, float lodThreshold)
	{
		mOctree->update(viewPosition, lodThreshold);
	}
}
