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
#include "Raycasting.h"
#include "SQLiteUtils.h"
#include "VoxelDatabase.h"

#include <boost/filesystem.hpp>

#include <stdlib.h>
#include <time.h>

namespace Cubiquity
{
	template <typename VoxelType>
	Volume<VoxelType>::Volume(const Region& region, const std::string& pathToNewVoxelDatabase, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
		,m_pVoxelDatabase(0)
		,mOctree(0)
		,mDatabase(0)
	{
		POLYVOX_THROW_IF(region.getWidthInVoxels() == 0, std::invalid_argument, "Volume width must be greater than zero");
		POLYVOX_THROW_IF(region.getHeightInVoxels() == 0, std::invalid_argument, "Volume height must be greater than zero");
		POLYVOX_THROW_IF(region.getDepthInVoxels() == 0, std::invalid_argument, "Volume depth must be greater than zero");

		//Note: If the file is NULL then we don't need to (and can't) close it.
		FILE* file = fopen(pathToNewVoxelDatabase.c_str(), "rb");
		if(file != NULL)
		{
			fclose(file);
			POLYVOX_THROW(std::invalid_argument, "Cannot create a new voxel database as the provided filename already exists");
		}

		logInfo() << "Creating VoxelDatabase from '" << pathToNewVoxelDatabase << "'";
		
		logInfo() << "Attempting to create '" << pathToNewVoxelDatabase << "'";
		EXECUTE_SQLITE_FUNC( sqlite3_open_v2(pathToNewVoxelDatabase.c_str(), &mDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) );
		logInfo() << "Successfully created'" << pathToNewVoxelDatabase << "'";

		// Disable syncing
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "PRAGMA synchronous = OFF", 0, 0, 0) );

		m_pVoxelDatabase = new VoxelDatabase<VoxelType>(mDatabase);
		
		mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pVoxelDatabase, m_pVoxelDatabase, 32);

		mPolyVoxVolume->setMaxNumberOfBlocksInMemory(256);
		mPolyVoxVolume->setMaxNumberOfUncompressedBlocks(64);
	}

	template <typename VoxelType>
	Volume<VoxelType>::Volume(const std::string& pathToExistingVoxelDatabase, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
		,m_pVoxelDatabase(0)
		,mOctree(0)
		,mDatabase(0)
	{
		Region region(0, 0, 0, 127, 31, 127); //HACK!!!!

		POLYVOX_THROW_IF(region.getWidthInVoxels() == 0, std::invalid_argument, "Volume width must be greater than zero");
		POLYVOX_THROW_IF(region.getHeightInVoxels() == 0, std::invalid_argument, "Volume height must be greater than zero");
		POLYVOX_THROW_IF(region.getDepthInVoxels() == 0, std::invalid_argument, "Volume depth must be greater than zero");

		logInfo() << "Creating VoxelDatabase from '" << pathToExistingVoxelDatabase << "'";
		
		// Open the database
		EXECUTE_SQLITE_FUNC(  sqlite3_open_v2(pathToExistingVoxelDatabase.c_str(), &mDatabase, SQLITE_OPEN_READWRITE, NULL) );
		logInfo() << "Successfully opened'" << pathToExistingVoxelDatabase << "'";

		// Disable syncing
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "PRAGMA synchronous = OFF", 0, 0, 0) );

		m_pVoxelDatabase = new VoxelDatabase<VoxelType>(mDatabase);
		
		mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pVoxelDatabase, m_pVoxelDatabase, 32);

		mPolyVoxVolume->setMaxNumberOfBlocksInMemory(256);
		mPolyVoxVolume->setMaxNumberOfUncompressedBlocks(64);
	}

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		logTrace() << "Entering ~Volume()";

		// NOTE: We should really delete the volume here, but the background task processor might still be using it.
		// We need a way to shut that down, or maybe smart pointers can help here. Just flush until we have a better fix.
		mPolyVoxVolume->flushAll();

		//delete mPolyVoxVolume;
		//delete m_pCompressor;

		m_pVoxelDatabase->acceptOverrideBlocks();
		delete m_pVoxelDatabase;


		EXECUTE_SQLITE_FUNC( sqlite3_close(mDatabase) );

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
