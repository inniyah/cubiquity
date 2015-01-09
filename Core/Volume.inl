#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Raycast.h"
#include "PolyVoxCore/VolumeResampler.h"

#include "PolyVoxCore/Impl/Utility.h" //Should we include from Impl?

#include "Clock.h"
#include "BackgroundTaskProcessor.h"
#include "Logging.h"
#include "MainThreadTaskProcessor.h"
#include "MaterialSet.h"
#include "Raycasting.h"
#include "SQLiteUtils.h"
#include "VoxelDatabase.h"

#include <stdlib.h>
#include <time.h>

namespace Cubiquity
{
	template <typename VoxelType>
	Volume<VoxelType>::Volume(const Region& region, const std::string& pathToNewVoxelDatabase, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
		,m_pVoxelDatabase(0)
		,mOctree(0)
		,mBackgroundTaskProcessor(0)
	{
		POLYVOX_THROW_IF(region.getWidthInVoxels() == 0, std::invalid_argument, "Volume width must be greater than zero");
		POLYVOX_THROW_IF(region.getHeightInVoxels() == 0, std::invalid_argument, "Volume height must be greater than zero");
		POLYVOX_THROW_IF(region.getDepthInVoxels() == 0, std::invalid_argument, "Volume depth must be greater than zero");

		//m_pVoxelDatabase = new VoxelDatabase<VoxelType>;
		//m_pVoxelDatabase->create(pathToNewVoxelDatabase);

		m_pVoxelDatabase = VoxelDatabase<VoxelType>::createEmpty(pathToNewVoxelDatabase);

		// Store the volume region to the database.
		m_pVoxelDatabase->setProperty("lowerX", region.getLowerX());
		m_pVoxelDatabase->setProperty("lowerY", region.getLowerY());
		m_pVoxelDatabase->setProperty("lowerZ", region.getLowerZ());
		m_pVoxelDatabase->setProperty("upperX", region.getUpperX());
		m_pVoxelDatabase->setProperty("upperY", region.getUpperY());
		m_pVoxelDatabase->setProperty("upperZ", region.getUpperZ());
		
		mPolyVoxVolume = new ::PolyVox::PagedVolume<VoxelType>(region, m_pVoxelDatabase, 32);

		mPolyVoxVolume->setMemoryUsageLimit(256 * 1024 * 1024);

		mBackgroundTaskProcessor = new BackgroundTaskProcessor();
	}

	template <typename VoxelType>
	Volume<VoxelType>::Volume(const std::string& pathToExistingVoxelDatabase, WritePermission writePermission, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
		,m_pVoxelDatabase(0)
		,mOctree(0)
		//,mDatabase(0)
		,mBackgroundTaskProcessor(0)
	{
		//m_pVoxelDatabase = new VoxelDatabase<VoxelType>;
		//m_pVoxelDatabase->open(pathToExistingVoxelDatabase);

		m_pVoxelDatabase = VoxelDatabase<VoxelType>::createFromVDB(pathToExistingVoxelDatabase, writePermission);

		// Get the volume region from the database. The default values
		// are fairly arbitrary as there is no sensible choice here.
		int32_t lowerX = m_pVoxelDatabase->getPropertyAsInt("lowerX", 0);
		int32_t lowerY = m_pVoxelDatabase->getPropertyAsInt("lowerY", 0);
		int32_t lowerZ = m_pVoxelDatabase->getPropertyAsInt("lowerZ", 0);
		int32_t upperX = m_pVoxelDatabase->getPropertyAsInt("upperX", 512);
		int32_t upperY = m_pVoxelDatabase->getPropertyAsInt("upperY", 512);
		int32_t upperZ = m_pVoxelDatabase->getPropertyAsInt("upperZ", 512);
		Region region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);
		
		mPolyVoxVolume = new ::PolyVox::PagedVolume<VoxelType>(region, m_pVoxelDatabase, 32);

		mPolyVoxVolume->setMemoryUsageLimit(64 * 1024 * 1024);

		mBackgroundTaskProcessor = new BackgroundTaskProcessor();
	}

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		POLYVOX_LOG_TRACE("Entering ~Volume()");

		delete mBackgroundTaskProcessor;
		mBackgroundTaskProcessor = 0;

		// NOTE: We should really delete the volume here, but the background task processor might still be using it.
		// We need a way to shut that down, or maybe smart pointers can help here. Just flush until we have a better fix.
		mPolyVoxVolume->flushAll();

		//delete mPolyVoxVolume;

		delete m_pVoxelDatabase;

		POLYVOX_LOG_TRACE("Exiting ~Volume()");
	}

	template <typename VoxelType>
	VoxelType Volume<VoxelType>::getVoxelAt(int32_t x, int32_t y, int32_t z) const
	{
		// Border value is returned for invalid position
		return mPolyVoxVolume->template getVoxel< ::PolyVox::WrapModes::Border>(x, y, z, VoxelType());
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxelAt(int32_t x, int32_t y, int32_t z, VoxelType value, bool markAsModified)
	{
		// Validate the voxel position
		POLYVOX_THROW_IF(mPolyVoxVolume->getEnclosingRegion().containsPoint(x, y, z) == false,
			std::invalid_argument, "Attempted to write to a voxel which is outside of the volume");

		mPolyVoxVolume->setVoxelAt(x, y, z, value);
		if(markAsModified)
		{
			mOctree->markDataAsModified(x, y, z, Clock::getTimestamp());
		}
	}

	template <typename VoxelType>
	void Volume<VoxelType>::markAsModified(const Region& region)
	{
		mOctree->markDataAsModified(region, Clock::getTimestamp());
	}

	template <typename VoxelType>
	bool Volume<VoxelType>::update(const Vector3F& viewPosition, float lodThreshold)
	{
		return mOctree->update(viewPosition, lodThreshold);
	}
}
