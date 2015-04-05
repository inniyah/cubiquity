#include "PolyVox/LowPassFilter.h"
#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/Raycast.h"
#include "PolyVox/VolumeResampler.h"

#include "PolyVox/Impl/Utility.h" //Should we include from Impl?

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

		mEnclosingRegion = region;

		m_pVoxelDatabase = VoxelDatabase<VoxelType>::createEmpty(pathToNewVoxelDatabase);

		// Store the volume region to the database.
		m_pVoxelDatabase->setProperty("lowerX", region.getLowerX());
		m_pVoxelDatabase->setProperty("lowerY", region.getLowerY());
		m_pVoxelDatabase->setProperty("lowerZ", region.getLowerZ());
		m_pVoxelDatabase->setProperty("upperX", region.getUpperX());
		m_pVoxelDatabase->setProperty("upperY", region.getUpperY());
		m_pVoxelDatabase->setProperty("upperZ", region.getUpperZ());

		// All new databases use Morton ordering. Support for linear ordering is only maintained for backwards compatability.
		m_pVoxelDatabase->setProperty("ordering", "morton");
		
		mPolyVoxVolume = new ::PolyVox::PagedVolume<VoxelType>(m_pVoxelDatabase, 256 * 1024 * 1024, 32);

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

		// All new databases use Morton ordering. Support for linear ordering is only maintained for backwards compatability.
		std::string ordering = m_pVoxelDatabase->getPropertyAsString("ordering", "linear");
		if (ordering == "linear")
		{
			POLYVOX_LOG_WARNING("This database is using the old 'linear' ordering for it's data, rather than the new 'morton' ordering.");
		}

		mEnclosingRegion = Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);
		
		mPolyVoxVolume = new ::PolyVox::PagedVolume<VoxelType>(m_pVoxelDatabase, 256 * 1024 * 1024, 32);

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
	VoxelType Volume<VoxelType>::getVoxel(int32_t x, int32_t y, int32_t z) const
	{
		return mPolyVoxVolume->getVoxel(x, y, z);
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setVoxel(int32_t x, int32_t y, int32_t z, VoxelType value, bool markAsModified)
	{
		// Validate the voxel position
		POLYVOX_THROW_IF(mEnclosingRegion.containsPoint(x, y, z) == false,
			std::invalid_argument, "Attempted to write to a voxel which is outside of the volume");

		mPolyVoxVolume->setVoxel(x, y, z, value);
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
