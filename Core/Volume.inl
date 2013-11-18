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
#include "MaterialSet.h"
#include "OverrideFilePager.h"
#include "Raycasting.h"
#include "SQLitePager.h"

#include <boost/filesystem.hpp>

#include <stdlib.h>
#include <time.h>

namespace Cubiquity
{
	template <typename VoxelType>
	Volume<VoxelType>::Volume(const Region& region, const std::string& filename, OctreeConstructionMode octreeConstructionMode, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
		,m_pCompressor(0)
		,m_pSQLitePager(0)
		,mOctree(0)
	{
		logTrace() << "Entering Volume(" << region << ",...)";

		/*POLYVOX_ASSERT(region.getWidthInVoxels() > 0, "All volume dimensions must be greater than zero");
		POLYVOX_ASSERT(region.getHeightInVoxels() > 0, "All volume dimensions must be greater than zero");
		POLYVOX_ASSERT(region.getDepthInVoxels() > 0, "All volume dimensions must be greater than zero");*/
		POLYVOX_THROW_IF(region.getWidthInVoxels() == 0, std::invalid_argument, "Volume width must be greater than zero");
		POLYVOX_THROW_IF(region.getHeightInVoxels() == 0, std::invalid_argument, "Volume height must be greater than zero");
		POLYVOX_THROW_IF(region.getDepthInVoxels() == 0, std::invalid_argument, "Volume depth must be greater than zero");

		POLYVOX_THROW_IF(filename.size() == 0, std::invalid_argument, "A valid filename must be provided");
	
		m_pCompressor = new ::PolyVox::MinizBlockCompressor<VoxelType>;

		m_pSQLitePager = new SQLitePager<VoxelType>(filename);
		
		//FIXME - This should not be decided based on the Octree type but instead be in different volume constructors
		if(octreeConstructionMode == OctreeConstructionModes::BoundCells) // Smooth terrain
		{
			mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pCompressor, m_pSQLitePager, 32);
		}
		else // Cubic terrain
		{
			mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pCompressor, m_pSQLitePager, 64);
		}

		mPolyVoxVolume->setMaxNumberOfBlocksInMemory(256);
		mPolyVoxVolume->setMaxNumberOfUncompressedBlocks(64);

		mOctree = new Octree<VoxelType>(this, octreeConstructionMode, baseNodeSize);

		logTrace() << "Leaving Volume(" << region << ",...)";
	}

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		logTrace() << "Entering ~Volume()";

		delete mOctree;

		// NOTE: We should really delete the volume here, but the background task processor might still be using it.
		// We need a way to shut that down, or maybe smart pointers can help here. Just flush until we have a better fix.
		mPolyVoxVolume->flushAll();

		//delete mPolyVoxVolume;
		//delete m_pCompressor;

		m_pSQLitePager->acceptOverrideBlocks();
		delete m_pSQLitePager;

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
