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

		POLYVOX_LOG_INFO("Creating VoxelDatabase from '" << pathToNewVoxelDatabase << "'");
		
		POLYVOX_LOG_INFO("Attempting to create '" << pathToNewVoxelDatabase << "'");
		EXECUTE_SQLITE_FUNC( sqlite3_open_v2(pathToNewVoxelDatabase.c_str(), &mDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) );
		POLYVOX_LOG_INFO("Successfully created'" << pathToNewVoxelDatabase << "'");

		// Disable syncing
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "PRAGMA synchronous = OFF", 0, 0, 0) );

		m_pVoxelDatabase = new VoxelDatabase<VoxelType>(mDatabase);
		
		mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pVoxelDatabase, m_pVoxelDatabase, 32);

		mPolyVoxVolume->setMaxNumberOfBlocksInMemory(256);
		mPolyVoxVolume->setMaxNumberOfUncompressedBlocks(64);

		// Create the 'Properties' table.
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "CREATE TABLE Properties(Name TEXT PRIMARY KEY, Value TEXT);", 0, 0, 0) );

		// Now build the 'select' and 'insert or replace' prepared statements
		EXECUTE_SQLITE_FUNC( sqlite3_prepare_v2(mDatabase, "SELECT Value FROM Properties WHERE Name = ?", -1, &mSelectPropertyStatement, NULL) );
		EXECUTE_SQLITE_FUNC( sqlite3_prepare_v2(mDatabase, "INSERT OR REPLACE INTO Properties (Name, Value) VALUES (?, ?)", -1, &mInsertOrReplacePropertyStatement, NULL) );		

		// Store the volume region to the database.
		setProperty("lowerX", region.getLowerX());
		setProperty("lowerY", region.getLowerY());
		setProperty("lowerZ", region.getLowerZ());
		setProperty("upperX", region.getUpperX());
		setProperty("upperY", region.getUpperY());
		setProperty("upperZ", region.getUpperZ());
	}

	template <typename VoxelType>
	Volume<VoxelType>::Volume(const std::string& pathToExistingVoxelDatabase, uint32_t baseNodeSize)
		:mPolyVoxVolume(0)
		,m_pVoxelDatabase(0)
		,mOctree(0)
		,mDatabase(0)
	{
		POLYVOX_LOG_INFO("Creating VoxelDatabase from '" << pathToExistingVoxelDatabase << "'");
		
		// Open the database
		EXECUTE_SQLITE_FUNC(  sqlite3_open_v2(pathToExistingVoxelDatabase.c_str(), &mDatabase, SQLITE_OPEN_READWRITE, NULL) );
		POLYVOX_LOG_INFO("Successfully opened'" << pathToExistingVoxelDatabase << "'");

		// Disable syncing
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "PRAGMA synchronous = OFF", 0, 0, 0) );

		// Now build the 'select' and 'insert or replace' prepared statements
		EXECUTE_SQLITE_FUNC( sqlite3_prepare_v2(mDatabase, "SELECT Value FROM Properties WHERE Name = ?", -1, &mSelectPropertyStatement, NULL) );
		EXECUTE_SQLITE_FUNC( sqlite3_prepare_v2(mDatabase, "INSERT OR REPLACE INTO Properties (Name, Value) VALUES (?, ?)", -1, &mInsertOrReplacePropertyStatement, NULL) );

		// Get the volume region from the database. The default values
		// are fairly arbitrary as there is no sensible choice here.
		int32_t lowerX = getPropertyAsInt("lowerX", 0);
		int32_t lowerY = getPropertyAsInt("lowerY", 0);
		int32_t lowerZ = getPropertyAsInt("lowerZ", 0);
		int32_t upperX = getPropertyAsInt("upperX", 512);
		int32_t upperY = getPropertyAsInt("upperY", 512);
		int32_t upperZ = getPropertyAsInt("upperZ", 512);
		Region region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);

		m_pVoxelDatabase = new VoxelDatabase<VoxelType>(mDatabase);
		
		mPolyVoxVolume = new ::PolyVox::LargeVolume<VoxelType>(region, m_pVoxelDatabase, m_pVoxelDatabase, 32);

		mPolyVoxVolume->setMaxNumberOfBlocksInMemory(256);
		mPolyVoxVolume->setMaxNumberOfUncompressedBlocks(64);
	}

	template <typename VoxelType>
	Volume<VoxelType>::~Volume()
	{
		POLYVOX_LOG_TRACE("Entering ~Volume()");

		// NOTE: We should really delete the volume here, but the background task processor might still be using it.
		// We need a way to shut that down, or maybe smart pointers can help here. Just flush until we have a better fix.
		mPolyVoxVolume->flushAll();

		//delete mPolyVoxVolume;
		//delete m_pCompressor;

		m_pVoxelDatabase->acceptOverrideBlocks();
		delete m_pVoxelDatabase;

		EXECUTE_SQLITE_FUNC( sqlite3_finalize(mSelectPropertyStatement) );
		EXECUTE_SQLITE_FUNC( sqlite3_finalize(mInsertOrReplacePropertyStatement) );

		POLYVOX_LOG_TRACE("Vacuuming database...");
		PolyVox::Timer timer;
		EXECUTE_SQLITE_FUNC( sqlite3_exec(mDatabase, "VACUUM;", 0, 0, 0) );
		POLYVOX_LOG_TRACE("Vacuumed database in " << timer.elapsedTimeInMilliSeconds() << "ms");

		EXECUTE_SQLITE_FUNC( sqlite3_close(mDatabase) );

		POLYVOX_LOG_TRACE("Exiting ~Volume()");
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

	template <typename VoxelType>
	bool Volume<VoxelType>::getProperty(const std::string& name, std::string& value)
	{
		EXECUTE_SQLITE_FUNC( sqlite3_reset(mSelectPropertyStatement) );
		EXECUTE_SQLITE_FUNC( sqlite3_bind_text(mSelectPropertyStatement, 1, name.c_str(), -1, SQLITE_TRANSIENT) );
		if(sqlite3_step(mSelectPropertyStatement) == SQLITE_ROW)
        {
			// I think the last index is zero because our select statement only returned one column.
            value = std::string( reinterpret_cast<const char*>( sqlite3_column_text(mSelectPropertyStatement, 0) ) );
			return true;
        }
		else
		{
			POLYVOX_LOG_WARNING("Property '" << name << "' was not found. The default value will be used instead");
			return false;
		}
	}

	template <typename VoxelType>
	int32_t Volume<VoxelType>::getPropertyAsInt(const std::string& name, int32_t defaultValue)
	{
		std::string value;
		if(getProperty(name, value))
		{
			return ::atol(value.c_str());
		}

		return defaultValue;
	}

	template <typename VoxelType>
	float Volume<VoxelType>::getPropertyAsFloat(const std::string& name, float defaultValue)
	{
		std::string value;
		if(getProperty(name, value))
		{
			return ::atof(value.c_str());
		}

		return defaultValue;
	}

	template <typename VoxelType>
	std::string Volume<VoxelType>::getPropertyAsString(const std::string& name, const std::string& defaultValue)
	{
		std::string value;
		if(getProperty(name, value))
		{
			return value;
		}

		return defaultValue;
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setProperty(const std::string& name, int value)
	{
		std::stringstream ss;
		ss << value;
		setProperty(name, ss.str());
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setProperty(const std::string& name, float value)
	{
		std::stringstream ss;
		ss << value;
		setProperty(name, ss.str());
	}

	template <typename VoxelType>
	void Volume<VoxelType>::setProperty(const std::string& name, const std::string& value)
	{
		// Based on: http://stackoverflow.com/a/5308188
		EXECUTE_SQLITE_FUNC( sqlite3_reset(mInsertOrReplacePropertyStatement) );
		EXECUTE_SQLITE_FUNC( sqlite3_bind_text(mInsertOrReplacePropertyStatement, 1, name.c_str(), -1, SQLITE_TRANSIENT) );
		EXECUTE_SQLITE_FUNC( sqlite3_bind_text(mInsertOrReplacePropertyStatement, 2, value.c_str(), -1, SQLITE_TRANSIENT) );
		sqlite3_step(mInsertOrReplacePropertyStatement); //Don't wrap this one as it isn't supposed to return SQLITE_OK?
	}
}
