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
#include "VoxelDatabase.h"

#include <boost/filesystem.hpp>

#include <stdlib.h>
#include <time.h>

namespace Cubiquity
{
	template <typename VoxelType>
	Volume<VoxelType>::Volume(const Region& region, const std::string& pathToVoxelDatabase, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
		,m_pVoxelDatabase(0)
		,mOctree(0)
		,mDatabase(0)
	{
		logTrace() << "Entering Volume(" << region << ",...)";

		POLYVOX_THROW_IF(region.getWidthInVoxels() == 0, std::invalid_argument, "Volume width must be greater than zero");
		POLYVOX_THROW_IF(region.getHeightInVoxels() == 0, std::invalid_argument, "Volume height must be greater than zero");
		POLYVOX_THROW_IF(region.getDepthInVoxels() == 0, std::invalid_argument, "Volume depth must be greater than zero");

		POLYVOX_THROW_IF(pathToVoxelDatabase.size() == 0, std::invalid_argument, "A valid voxel database path must be provided");

		logInfo() << "Creating VoxelDatabase from '" << pathToVoxelDatabase << "'";

		int rc = 0; // SQLite return code
		char* pErrorMsg = 0; // SQLite error message
		
		// Open the database if it already exists.
		rc = sqlite3_open_v2(pathToVoxelDatabase.c_str(), &mDatabase, SQLITE_OPEN_READWRITE, NULL);
		if(rc != SQLITE_OK)
		{
			logInfo() << "Failed to open '" << pathToVoxelDatabase << "'. Error message was: \"" << sqlite3_errmsg(mDatabase) << "\"";

			// If we failed, then try again but this time allow it to be created.
			logInfo() << "Attempting to create '" << pathToVoxelDatabase << "'";
			rc = sqlite3_open_v2(pathToVoxelDatabase.c_str(), &mDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
			if(rc != SQLITE_OK)
			{
				// If we failed to create it as well then we give up
				std::stringstream ss;
				ss << "Failed to create '" << pathToVoxelDatabase << "'. Error message was: \"" << sqlite3_errmsg(mDatabase) << "\"";
				throw std::runtime_error(ss.str().c_str());
			}
			logInfo() << "Successfully created'" << pathToVoxelDatabase << "'";
		}
		else
		{
			logInfo() << "Successfully opened'" << pathToVoxelDatabase << "'";
		}

		// Disable syncing
		rc = sqlite3_exec(mDatabase, "PRAGMA synchronous = OFF", 0, 0, &pErrorMsg);
		if(rc != SQLITE_OK)
		{
			std::stringstream ss;
			ss << "Failed to set 'synchronous' to OFF. Message was: \"" << pErrorMsg << "\"";
			sqlite3_free(pErrorMsg);
			throw std::runtime_error(ss.str().c_str());
		}
	
		//m_pCompressor = new ::PolyVox::MinizBlockCompressor<VoxelType>;

		m_pVoxelDatabase = new VoxelDatabase<VoxelType>(mDatabase);
		
		mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pVoxelDatabase, m_pVoxelDatabase, 32);

		mPolyVoxVolume->setMaxNumberOfBlocksInMemory(256);
		mPolyVoxVolume->setMaxNumberOfUncompressedBlocks(64);

		logTrace() << "Leaving Volume(" << region << ",...)";
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

		logInfo() << "Closing database connection...";
		int rc = sqlite3_close(mDatabase);
		if(rc == SQLITE_OK)
		{
			logInfo() << "Connection closed successfully";
		}
		else
		{
			logInfo() << "Error closing connection. Error message was: \"" << sqlite3_errstr(rc) << "\"";
		}

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
