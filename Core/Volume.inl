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
	Volume<VoxelType>::Volume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize)
		:mPolyVoxVolume(0)
		,mOctree(0)
	{
		::PolyVox::Region volumeRegion(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);

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
	VoxelType Volume<VoxelType>::getVoxelAt(int x, int y, int z)
	{
		return mPolyVoxVolume->getVoxelAt(x, y, z);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(int x, int y, int z, VoxelType value, UpdatePriority updatePriority)
	{
		mPolyVoxVolume->setVoxelAt(x, y, z, value);
		if(updatePriority != UpdatePriorities::DontUpdate)
		{
			mOctree->mRootOctreeNode->markAsModified(x, y, z, Clock::getTimestamp(), updatePriority);
		}
	}

	template <typename VoxelType>
	void Volume<VoxelType>::markAsModified(const ::PolyVox::Region& region, UpdatePriority updatePriority)
	{
		POLYVOX_ASSERT(updatePriority != UpdatePriorities::DontUpdate, "You cannot mark as modified yet request no update");
		mOctree->mRootOctreeNode->markAsModified(region, Clock::getTimestamp(), updatePriority);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::update(const ::PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
	{
		mOctree->update(viewPosition, lodThreshold);
	}
}
