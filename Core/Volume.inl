#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Raycast.h"
#include "PolyVoxCore/VolumeResampler.h"

#include "PolyVoxCore/Impl/Utility.h" //Should we include from Impl?

#include "Clock.h"
#include "BackgroundTaskProcessor.h"
#include "MainThreadTaskProcessor.h"
#include "MultiMaterial.h"

#include "Raycasting.h"

namespace Cubiquity
{
	template <typename VoxelType>
	Volume<VoxelType>::Volume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, uint32_t blockSize, OctreeConstructionMode octreeConstructionMode, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
		,mOctree(0)
	{
		Region volumeRegion(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);

		POLYVOX_ASSERT(volumeRegion.getWidthInVoxels() > 0, "All volume dimensions must be greater than zero");
		POLYVOX_ASSERT(volumeRegion.getHeightInVoxels() > 0, "All volume dimensions must be greater than zero");
		POLYVOX_ASSERT(volumeRegion.getDepthInVoxels() > 0, "All volume dimensions must be greater than zero");
	
		mPolyVoxVolume = new ::PolyVox::SimpleVolume<VoxelType>(volumeRegion);

		mOctree = new Octree<VoxelType>(this, octreeConstructionMode, baseNodeSize);
	}

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(int32_t x, int32_t y, int32_t z, VoxelType value, UpdatePriority updatePriority)
	{
		mPolyVoxVolume->setVoxelAt(x, y, z, value);
		if(updatePriority != UpdatePriorities::DontUpdate)
		{
			mOctree->mRootOctreeNode->markAsModified(x, y, z, Clock::getTimestamp(), updatePriority);
		}
	}

	template <typename VoxelType>
	void Volume<VoxelType>::markAsModified(const Region& region, UpdatePriority updatePriority)
	{
		POLYVOX_ASSERT(updatePriority != UpdatePriorities::DontUpdate, "You cannot mark as modified yet request no update");
		mOctree->mRootOctreeNode->markAsModified(region, Clock::getTimestamp(), updatePriority);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::update(const Vector3F& viewPosition, float lodThreshold)
	{
		mOctree->update(viewPosition, lodThreshold);
	}
}
