// CubiquityC.cpp : Defines the exported functions for the DLL application.
//

#include "CubiquityC.h"

#include "Brush.h"
#include "ColoredCubesVolume.h"
#include "Logging.h"
#include "OctreeNode.h"
#include "Raycasting.h"
#include "TerrainVolume.h"
#include "TerrainVolumeEditor.h"
#include "TerrainVolumeGenerator.h"

#if defined (_MSC_VER) || defined(__APPLE__)
	#include <future> //For std::future_error, but causes chrono-related compile errors on Linux/GCC.
#endif
#include <new>
#include <stdexcept>
#include <vector>

using namespace std;

const uint32_t CuMajorVersion = 1;
const uint32_t CuMinorVersion = 1;
const uint32_t CuPatchVersion = 4;

char gLastErrorMessage[4096];

#define CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(exception, errorCode) \
catch (const exception& ex) \
{ \
	POLYVOX_LOG_ERROR("Caught \'" #exception "\' at C interface. Message reads: \"" << ex.what() << "\""); \
	strcpy(gLastErrorMessage, ex.what()); \
	return errorCode; \
}

#define OPEN_C_INTERFACE \
	try \
	{

#define CLOSE_C_INTERFACE \
	return CU_OK; \
	} \
	/* Note - Exceptions are ordered from most to least specific */ \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(DatabaseError, CU_DATABASE_ERROR) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(CompressionError, CU_COMPRESSION_ERROR) \
	\
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(ios_base::failure, CU_IOS_BASE_FAILURE) \
	\
	/*CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(bad_array_new_length, CU_BAD_ARRAY_NEW_LENGTH) *Causing compile errors on Linux/GCC* */ \
	\
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(underflow_error, CU_UNDERFLOW_ERROR) \
	/*CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(system_error, CU_SYSTEM_ERROR) *Causing compile errors on Linux/GCC* */ \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(range_error, CU_RANGE_ERROR) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(overflow_error, CU_OVERFLOW_ERROR) \
	\
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(out_of_range, CU_OUT_OF_RANGE) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(length_error, CU_LENGTH_ERROR) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(invalid_argument, CU_INVALID_ARGUMENT) \
	/*CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(future_error, CU_FUTURE_ERROR) *Requires <future> header (problematic on Linux/GCC)* */ \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(domain_error, CU_DOMAIN_ERROR) \
	\
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(runtime_error, CU_RUNTIME_ERROR) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(logic_error, CU_LOGIC_ERROR) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(bad_weak_ptr, CU_BAD_WEAK_PTR) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(bad_typeid, CU_BAD_TYPEID) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(bad_function_call, CU_BAD_FUNCTION_CALL) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(bad_exception, CU_BAD_EXCEPTION) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(bad_cast, CU_BAD_CAST) \
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(bad_alloc, CU_BAD_ALLOC) \
	\
	CATCH_EXCEPTION_AND_MAP_TO_ERROR_CODE(exception, CU_EXCEPTION) \
	\
	catch (...) \
	{ \
		POLYVOX_LOG_ERROR("Caught unknown exception at C interface."); \
		strcpy(gLastErrorMessage, "No error message due to unknown exception type."); \
		return CU_UNKNOWN_ERROR; \
	} \

using namespace Cubiquity;

const int MaxNoOfVolumes = 256;
const int TotalHandleBits = 32;
const int VolumeHandleBits = 8;
const int MaxVolumeHandle = (0x01 << VolumeHandleBits) - 1;

const int NodeHandleBits = 16; // Set this properly later.
const int NodeHandleMask = (0x01 << NodeHandleBits) - 1;
const int MaxNodeHandle = (0x01 << NodeHandleBits) - 1;

void* gVolumes[MaxNoOfVolumes];

// This class (via it's single global instance) allows code to be executed when the library is loaded and unloaded.
// I do have some concerns about how robust this is - in particular see here: http://stackoverflow.com/a/1229542
class EntryAndExitPoints
{
public:
	EntryAndExitPoints()
		:mFileLogger()
	{
		PolyVox::setLogger(&mFileLogger);

		// HACK - Should have a seperate init function for this?
		for (int ct = 0; ct < MaxNoOfVolumes; ct++)
		{
			gVolumes[ct] = 0;
		}
	}

	~EntryAndExitPoints()
	{
		PolyVox::setLogger(0);
	}

public:
	FileLogger mFileLogger;
};

// The single global instance of the above class.
EntryAndExitPoints gEntryAndExitPoints;

void* getVolumeFromHandle(uint32_t volumeHandle)
{
	return gVolumes[volumeHandle];
}

ColoredCubesVolume* getColoredCubesVolumeFromHandle(uint32_t volumeIndex)
{
	ColoredCubesVolume* volume = reinterpret_cast<ColoredCubesVolume*>(gVolumes[volumeIndex]);
	return volume;
}

TerrainVolume* getTerrainVolumeFromHandle(uint32_t volumeIndex)
{
	TerrainVolume* volume = reinterpret_cast<TerrainVolume*>(gVolumes[volumeIndex]);
	return volume;
}

uint32_t encodeHandle(uint32_t volumeType, uint32_t volumeIndex, uint32_t nodeIndex)
{
	uint32_t handle = volumeType << (TotalHandleBits - 1);
	handle |= (volumeIndex << NodeHandleBits);
	handle |= nodeIndex;
	return handle;
}

void decodeHandle(uint32_t handle, uint32_t* volumeType, uint32_t* volumeIndex, uint32_t* nodeIndex)
{
	*volumeType = handle >> (TotalHandleBits - 1);
	*volumeIndex = (handle >> NodeHandleBits) & 0xFF;
	*nodeIndex = handle & NodeHandleMask;
}

void* getNode(uint32_t volumeType, uint32_t volumeIndex, uint32_t nodeIndex)
{
	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		OctreeNode<Color>* node = volume->getOctree()->getNodeFromIndex(nodeIndex);
		return node;
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		OctreeNode<MaterialSet>* node = volume->getOctree()->getNodeFromIndex(nodeIndex);
		return node;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Version functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuGetVersionNumber(uint32_t* majorVersion, uint32_t* minorVersion, uint32_t* patchVersion)
{
	OPEN_C_INTERFACE

	*majorVersion = CuMajorVersion;
	*minorVersion = CuMinorVersion;
	*patchVersion = CuPatchVersion;

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Logging functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API const char* cuGetLogFilePath(void)
{
	// Use of buffer is a bit of a hack! But otherwise the string doesn't come through. To be investigated.
	static char buffer[1024];
	strcpy(buffer, gEntryAndExitPoints.mFileLogger.getLogFilePath().c_str());
	return buffer;
}

#define ERROR_CODE_TO_STRING(errorCode) case errorCode: strcpy(buffer, #errorCode); break;

////////////////////////////////////////////////////////////////////////////////
// Logging functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API const char* cuGetErrorCodeAsString(int32_t errorCode)
{
	static char buffer[1024]; // Can't return a pointer to a local variableso we make it static.

	switch (errorCode)
	{
		ERROR_CODE_TO_STRING(CU_OK)

		ERROR_CODE_TO_STRING(CU_EXCEPTION)
		ERROR_CODE_TO_STRING(CU_BAD_ALLOC)
		ERROR_CODE_TO_STRING(CU_BAD_CAST)
		ERROR_CODE_TO_STRING(CU_BAD_EXCEPTION)
		ERROR_CODE_TO_STRING(CU_BAD_FUNCTION_CALL)
		ERROR_CODE_TO_STRING(CU_BAD_TYPEID)
		ERROR_CODE_TO_STRING(CU_BAD_WEAK_PTR)
		ERROR_CODE_TO_STRING(CU_LOGIC_ERROR)
		ERROR_CODE_TO_STRING(CU_RUNTIME_ERROR)

		ERROR_CODE_TO_STRING(CU_DOMAIN_ERROR)
		ERROR_CODE_TO_STRING(CU_FUTURE_ERROR)
		ERROR_CODE_TO_STRING(CU_INVALID_ARGUMENT)
		ERROR_CODE_TO_STRING(CU_LENGTH_ERROR)
		ERROR_CODE_TO_STRING(CU_OUT_OF_RANGE)

		ERROR_CODE_TO_STRING(CU_OVERFLOW_ERROR)
		ERROR_CODE_TO_STRING(CU_RANGE_ERROR)
		ERROR_CODE_TO_STRING(CU_SYSTEM_ERROR)
		ERROR_CODE_TO_STRING(CU_UNDERFLOW_ERROR)

		ERROR_CODE_TO_STRING(CU_BAD_ARRAY_NEW_LENGTH)

		ERROR_CODE_TO_STRING(CU_IOS_BASE_FAILURE)

		ERROR_CODE_TO_STRING(CU_DATABASE_ERROR)
		ERROR_CODE_TO_STRING(CU_COMPRESSION_ERROR)

		ERROR_CODE_TO_STRING(CU_UNKNOWN_ERROR)

	default:
		strcpy(buffer, "Unrecognised error code");
		break;
	}

	return buffer;
}

CUBIQUITYC_API const char* cuGetLastErrorMessage(void)
{
	return gLastErrorMessage;
}

////////////////////////////////////////////////////////////////////////////////
// Color functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API uint8_t cuGetRed(CuColor color)
{
	BitField<uint32_t> bits(color.data);
	return static_cast<uint8_t>(bits.getBits(Color::RedMSB, Color::RedLSB) * Color::RedScaleFactor);
}

CUBIQUITYC_API uint8_t cuGetGreen(CuColor color)
{
	BitField<uint32_t> bits(color.data);
	return static_cast<uint8_t>(bits.getBits(Color::GreenMSB, Color::GreenLSB) * Color::GreenScaleFactor);
}

CUBIQUITYC_API uint8_t cuGetBlue(CuColor color)
{
	BitField<uint32_t> bits(color.data);
	return static_cast<uint8_t>(bits.getBits(Color::BlueMSB, Color::BlueLSB) * Color::BlueScaleFactor);
}

CUBIQUITYC_API uint8_t cuGetAlpha(CuColor color)
{
	BitField<uint32_t> bits(color.data);
	return static_cast<uint8_t>(bits.getBits(Color::AlphaMSB, Color::AlphaLSB) * Color::AlphaScaleFactor);
}

CUBIQUITYC_API void cuGetAllComponents(CuColor color, uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t* alpha)
{
	BitField<uint32_t> bits(color.data);
	*red = static_cast<uint8_t>(bits.getBits(Color::RedMSB, Color::RedLSB) * Color::RedScaleFactor);
	*green = static_cast<uint8_t>(bits.getBits(Color::GreenMSB, Color::GreenLSB) * Color::GreenScaleFactor);
	*blue = static_cast<uint8_t>(bits.getBits(Color::BlueMSB, Color::BlueLSB) * Color::BlueScaleFactor);
	*alpha = static_cast<uint8_t>(bits.getBits(Color::AlphaMSB, Color::AlphaLSB) * Color::AlphaScaleFactor);
}

CUBIQUITYC_API CuColor cuMakeColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	BitField<uint32_t> bits;
	bits.setBits(Color::RedMSB, Color::RedLSB, red / Color::RedScaleFactor);
	bits.setBits(Color::GreenMSB, Color::GreenLSB, green / Color::GreenScaleFactor);
	bits.setBits(Color::BlueMSB, Color::BlueLSB, blue / Color::BlueScaleFactor);
	bits.setBits(Color::AlphaMSB, Color::AlphaLSB, alpha / Color::AlphaScaleFactor);

	CuColor color;
	color.data = bits.allBits();
	return color;
}

////////////////////////////////////////////////////////////////////////////////
// Volume functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuNewEmptyColoredCubesVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, const char* pathToNewVoxelDatabase, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColoredCubesVolume* volume = new ColoredCubesVolume(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), pathToNewVoxelDatabase, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.

	// Replace an existing entry if it has been deleted.
	bool foundEmptySlot = false;
	uint32_t ct = 0;
	for (; ct < MaxNoOfVolumes; ct++)
	{
		if(gVolumes[ct] == 0)
		{
			gVolumes[ct] = volume;
			foundEmptySlot = true;
			break;
		}
	}

	POLYVOX_THROW_IF(!foundEmptySlot, std::invalid_argument, "Cannot create new volume as there is a limit of " + MaxNoOfVolumes);

	POLYVOX_LOG_DEBUG("Created new colored cubes volume in slot " << ct);

	// Build the handle
	*result = encodeHandle(CU_COLORED_CUBES, ct, 0);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNewColoredCubesVolumeFromVDB(const char* pathToExistingVoxelDatabase, uint32_t writePermissions, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	// Fixme - Find out how to pass this write permissions enum properly.
	WritePermission cubiquityWritePermissions = (writePermissions == CU_READONLY) ? WritePermissions::ReadOnly : WritePermissions::ReadWrite;
	ColoredCubesVolume* volume = new ColoredCubesVolume(pathToExistingVoxelDatabase, cubiquityWritePermissions, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.

	// Replace an existing entry if it has been deleted.
	bool foundEmptySlot = false;
	uint32_t ct = 0;
	for (; ct < MaxNoOfVolumes; ct++)
	{
		if(gVolumes[ct] == 0)
		{
			gVolumes[ct] = volume;
			foundEmptySlot = true;
			break;
		}
	}

	POLYVOX_THROW_IF(!foundEmptySlot, std::invalid_argument, "Cannot create new volume as there is a limit of " + MaxNoOfVolumes);

	POLYVOX_LOG_DEBUG("Created new colored cubes volume in slot " << ct);

	// Build the handle
	*result = encodeHandle(CU_COLORED_CUBES, ct, 0);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuUpdateVolume(uint32_t volumeHandle, float eyePosX, float eyePosY, float eyePosZ, float lodThreshold)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		volume->update(Vector3F(eyePosX, eyePosY, eyePosZ), lodThreshold);
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		volume->update(Vector3F(eyePosX, eyePosY, eyePosZ), lodThreshold);
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuDeleteVolume(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	POLYVOX_LOG_DEBUG("Deleting volume with index " << volumeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		delete volume;
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		delete volume;
	}	

	// Set the slot to zero so that it can be reused.
	gVolumes[volumeIndex] = 0;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetEnclosingRegion(uint32_t volumeHandle, int32_t* lowerX, int32_t* lowerY, int32_t* lowerZ, int32_t* upperX, int32_t* upperY, int32_t* upperZ)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* coloredCubesVolume = getColoredCubesVolumeFromHandle(volumeIndex);
		const Region& region = coloredCubesVolume->getEnclosingRegion();

		*lowerX = region.getLowerCorner().getX();
		*lowerY = region.getLowerCorner().getY();
		*lowerZ = region.getLowerCorner().getZ();
		*upperX = region.getUpperCorner().getX();
		*upperY = region.getUpperCorner().getY();
		*upperZ = region.getUpperCorner().getZ();
	}
	else
	{
		TerrainVolume* terrainVolume = getTerrainVolumeFromHandle(volumeIndex);
		const Region& region = terrainVolume->getEnclosingRegion();

		*lowerX = region.getLowerCorner().getX();
		*lowerY = region.getLowerCorner().getY();
		*lowerZ = region.getLowerCorner().getZ();
		*upperX = region.getUpperCorner().getX();
		*upperY = region.getUpperCorner().getY();
		*upperZ = region.getUpperCorner().getZ();
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, void* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		Color temp = volume->getVoxelAt(x, y, z);
		CuColor* ptr = (CuColor*)&temp;
		CuColor* resultAsColor = (CuColor*)result;
		*resultAsColor = *ptr;
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		MaterialSet material = volume->getVoxelAt(x, y, z);
		CuMaterialSet* resultAsMaterialSet = (CuMaterialSet*)result;
		resultAsMaterialSet->data = material.mWeights.allBits();
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuSetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, void* value)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		Color* pColor = (Color*)value;
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		volume->setVoxelAt(x, y, z, *pColor, UpdatePriorities::Immediate);
	}
	else
	{
		MaterialSet* pMat = (MaterialSet*)value;
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		volume->setVoxelAt(x, y, z, *pMat, UpdatePriorities::Immediate);
	}
	
	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuAcceptOverrideChunks(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		volume->acceptOverrideChunks();
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		volume->acceptOverrideChunks();
	}
	
	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuDiscardOverrideChunks(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		volume->discardOverrideChunks();
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		volume->discardOverrideChunks();
	}
	
	CLOSE_C_INTERFACE
}

//--------------------------------------------------------------------------------

CUBIQUITYC_API int32_t cuNewEmptyTerrainVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, const char* pathToNewVoxelDatabase, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = new TerrainVolume(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), pathToNewVoxelDatabase, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.

	// Replace an existing entry if it has been deleted.
	bool foundEmptySlot = false;
	uint32_t ct = 0;
	for (; ct < MaxNoOfVolumes; ct++)
	{
		if(gVolumes[ct] == 0)
		{
			gVolumes[ct] = volume;
			foundEmptySlot = true;
			break;
		}
	}

	POLYVOX_THROW_IF(!foundEmptySlot, std::invalid_argument, "Cannot create new volume as there is a limit of " + MaxNoOfVolumes);

	POLYVOX_LOG_DEBUG("Created new smooth volume in slot " << ct);

	// Build the handle
	*result = encodeHandle(CU_TERRAIN, ct, 0);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNewTerrainVolumeFromVDB(const char* pathToExistingVoxelDatabase, uint32_t writePermissions, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	// Fixme - Find out how to pass this write permissions enum properly.
	WritePermission cubiquityWritePermissions = (writePermissions == CU_READONLY) ? WritePermissions::ReadOnly : WritePermissions::ReadWrite;
	TerrainVolume* volume = new TerrainVolume(pathToExistingVoxelDatabase, cubiquityWritePermissions, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.

	// Replace an existing entry if it has been deleted.
	bool foundEmptySlot = false;
	uint32_t ct = 0;
	for (; ct < MaxNoOfVolumes; ct++)
	{
		if(gVolumes[ct] == 0)
		{
			gVolumes[ct] = volume;
			foundEmptySlot = true;
			break;
		}
	}

	POLYVOX_THROW_IF(!foundEmptySlot, std::invalid_argument, "Cannot create new volume as there is a limit of " + MaxNoOfVolumes);

	POLYVOX_LOG_DEBUG("Created new smooth volume in slot " << ct);

	// Build the handle
	*result = encodeHandle(CU_TERRAIN, ct, 0);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVolumeType(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, result, &volumeIndex, &nodeIndex);

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Octree functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuHasRootOctreeNode(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		OctreeNode<Color>* node = volume->getRootOctreeNode();
		*result = node ? 1 : 0;
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		OctreeNode<MaterialSet>* node = volume->getRootOctreeNode();
		*result = node ? 1 : 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetRootOctreeNode(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);
		OctreeNode<Color>* node = volume->getRootOctreeNode();

		if (!node)
		{
			POLYVOX_THROW(PolyVox::invalid_operation, "No root node exists! Please check this with cuHasRootOctreeNode() first");
		}

		uint32_t nodeIndex = node->mSelf;

		*result = encodeHandle(CU_COLORED_CUBES, volumeIndex, nodeIndex);
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);
		OctreeNode<MaterialSet>* node = volume->getRootOctreeNode();

		if (!node)
		{
			POLYVOX_THROW(PolyVox::invalid_operation, "No root node exists! Please check this with cuHasRootOctreeNode() first");
		}

		uint32_t nodeIndex = node->mSelf;

		*result = encodeHandle(CU_TERRAIN, volumeIndex, nodeIndex);
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuHasChildNode(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		OctreeNode<Color>* child = node->getChildNode(childX, childY, childZ);
		*result = child ? 1 : 0;
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		OctreeNode<MaterialSet>* child = node->getChildNode(childX, childY, childZ);
		*result = child ? 1 : 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetChildNode(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		OctreeNode<Color>* child = node->getChildNode(childX, childY, childZ);

		if(!node)
		{
			POLYVOX_THROW(PolyVox::invalid_operation, "The specified child node does not exist! Please check this with cuHasChildNode() first");
		}

		uint32_t nodeIndex = child->mSelf;

		uint32_t volumeHandle;
		uint32_t dummy;
		decodeHandle(nodeHandle, &volumeType, &volumeHandle, &dummy);

		*result = encodeHandle(CU_COLORED_CUBES, volumeHandle, nodeIndex);
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		OctreeNode<MaterialSet>* child = node->getChildNode(childX, childY, childZ);

		if (!node)
		{
			POLYVOX_THROW(PolyVox::invalid_operation, "The specified child node does not exist! Please check this with cuHasChildNode() first");
		}

		uint32_t nodeIndex = child->mSelf;

		uint32_t volumeHandle;
		uint32_t dummy;
		decodeHandle(nodeHandle, &volumeType, &volumeHandle, &dummy);

		*result = encodeHandle(CU_TERRAIN, volumeHandle, nodeIndex);
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNodeHasMesh(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		*result = (node->mPolyVoxMesh != 0) ? 1 : 0;
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		*result = (node->mPolyVoxMesh != 0) ? 1 : 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNodePosition(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const Vector3I& lowerCorner = node->mRegion.getLowerCorner();
		*x = lowerCorner.getX();
		*y = lowerCorner.getY();
		*z = lowerCorner.getZ();
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const Vector3I& lowerCorner = node->mRegion.getLowerCorner();
		*x = lowerCorner.getX();
		*y = lowerCorner.getY();
		*z = lowerCorner.getZ();
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetMeshLastUpdated(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		*result = node->mMeshLastUpdated;
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		*result = node->mMeshLastUpdated;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuRenderThisNode(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		*result = node->mRenderThisNode;
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		*result = node->mRenderThisNode;
	}

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Mesh functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuGetNoOfVertices(uint32_t nodeHandle, uint16_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;
		*result = polyVoxMesh->getNoOfVertices();
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;
		*result = polyVoxMesh->getNoOfVertices();
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNoOfIndices(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;
		*result = polyVoxMesh->getNoOfIndices();
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;
		*result = polyVoxMesh->getNoOfIndices();
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVertices(uint32_t nodeHandle, void** result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;
		const std::vector< typename VoxelTraits<Color>::VertexType >& vertexVector = polyVoxMesh->getVertices();
		const VoxelTraits<Color>::VertexType* vertexPointer = &(vertexVector[0]);
		const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);
		float* floatPointer = const_cast<float*>(constFloatPointer);
		*result = floatPointer;
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;
		const std::vector< typename VoxelTraits<MaterialSet>::VertexType >& vertexVector = polyVoxMesh->getVertices();
		const VoxelTraits<MaterialSet>::VertexType* vertexPointer = &(vertexVector[0]);
		const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);
		float* floatPointer = const_cast<float*>(constFloatPointer);
		*result = floatPointer;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetIndices(uint32_t nodeHandle, uint16_t** result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;
		const std::vector< uint16_t >& indexVector = polyVoxMesh->getIndices();
		const uint16_t* constUInt16Pointer = &(indexVector[0]);
		uint16_t* uintPointer = const_cast<uint16_t*>(constUInt16Pointer);
		*result = uintPointer;
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));
		const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;
		const std::vector< uint16_t >& indexVector = polyVoxMesh->getIndices();
		const uint16_t* constUIntPointer = &(indexVector[0]);
		uint16_t* uintPointer = const_cast<uint16_t*>(constUIntPointer);
		*result = uintPointer;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetMesh(uint32_t nodeHandle, uint16_t* noOfVertices, void** vertices, uint32_t* noOfIndices, uint16_t** indices)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(nodeHandle, &volumeType, &volumeIndex, &nodeIndex);

	if (volumeType == CU_COLORED_CUBES)
	{
		// Get the node
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNode(volumeType, volumeIndex, nodeIndex));

		// Get the mesh
		const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

		// Get no of vertices
		*noOfVertices = polyVoxMesh->getNoOfVertices();

		// Get the vertices
		const std::vector< typename VoxelTraits<Color>::VertexType >& vertexVector = polyVoxMesh->getVertices();
		const VoxelTraits<Color>::VertexType* vertexPointer = &(vertexVector[0]);
		const void* constVoidPointer = reinterpret_cast<const void*>(vertexPointer);
		void* voidPointer = const_cast<void*>(constVoidPointer);
		*vertices = voidPointer;

		// Get no of indices
		*noOfIndices = polyVoxMesh->getNoOfIndices();

		// Get the indices
		const std::vector< uint16_t >& indexVector = polyVoxMesh->getIndices();
		const uint16_t* constUInt16Pointer = &(indexVector[0]);
		uint16_t* uintPointer = const_cast<uint16_t*>(constUInt16Pointer);
		*indices = uintPointer;
	}
	else
	{
		// Get the node
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNode(volumeType, volumeIndex, nodeIndex));

		// Get the mesh
		const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

		// Get no of vertices
		*noOfVertices = polyVoxMesh->getNoOfVertices();

		// Get the vertices
		const std::vector< typename VoxelTraits<MaterialSet>::VertexType >& vertexVector = polyVoxMesh->getVertices();
		const VoxelTraits<MaterialSet>::VertexType* vertexPointer = &(vertexVector[0]);
		const void* constVoidPointer = reinterpret_cast<const void*>(vertexPointer);
		void* voidPointer = const_cast<void*>(constVoidPointer);
		*vertices = voidPointer;

		// Get no of indices
		*noOfIndices = polyVoxMesh->getNoOfIndices();

		// Get the indices
		const std::vector< uint16_t >& indexVector = polyVoxMesh->getIndices();
		const uint16_t* constUIntPointer = &(indexVector[0]);
		uint16_t* uintPointer = const_cast<uint16_t*>(constUIntPointer);
		*indices = uintPointer;
	}

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Clock functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuGetCurrentTime(uint32_t* result)
{
	OPEN_C_INTERFACE

	*result = Clock::getTimestamp();

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Raycasting functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuPickFirstSolidVoxel(uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);
	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);

	if(pickFirstSolidVoxel(volume, rayStartX, rayStartY, rayStartZ, rayDirX, rayDirY, rayDirZ, resultX, resultY, resultZ))
	{
		*result = 1;
	}
	else
	{
		*result = 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuPickLastEmptyVoxel(uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);
	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeIndex);

	if(pickLastEmptyVoxel(volume, rayStartX, rayStartY, rayStartZ, rayDirX, rayDirY, rayDirZ, resultX, resultY, resultZ))
	{
		*result = 1;
	}
	else
	{
		*result = 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuPickTerrainSurface(uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, float* resultX, float* resultY, float* resultZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);
	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);

	if(pickTerrainSurface(volume, rayStartX, rayStartY, rayStartZ, rayDirX, rayDirY, rayDirZ, resultX, resultY, resultZ))
	{
		*result = 1;
	}
	else
	{
		*result = 0;
	}

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Editing functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuSculptTerrainVolume(uint32_t volumeHandle, float brushX, float brushY, float brushZ, float brushInnerRadius, float brushOuterRadius, float opacity)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);
	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);

	sculptTerrainVolume(volume, Vector3F(brushX, brushY, brushZ), Brush(brushInnerRadius, brushOuterRadius, opacity));

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuBlurTerrainVolume(uint32_t volumeHandle, float brushX, float brushY, float brushZ, float brushInnerRadius, float brushOuterRadius, float opacity)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);
	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);

	blurTerrainVolume(volume, Vector3F(brushX, brushY, brushZ), Brush(brushInnerRadius, brushOuterRadius, opacity));

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuBlurTerrainVolumeRegion(uint32_t volumeHandle, int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);
	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);

	blurTerrainVolume(volume, Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ));

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuPaintTerrainVolume(uint32_t volumeHandle, float brushX, float brushY, float brushZ, float brushInnerRadius, float brushOuterRadius, float opacity, uint32_t materialIndex)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);
	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);

	paintTerrainVolume(volume, Vector3F(brushX, brushY, brushZ), Brush(brushInnerRadius, brushOuterRadius, opacity), materialIndex);

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Volume generation functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuGenerateFloor(uint32_t volumeHandle, int32_t lowerLayerHeight, uint32_t lowerLayerMaterial, int32_t upperLayerHeight, uint32_t upperLayerMaterial)
{
	OPEN_C_INTERFACE

	uint32_t volumeType, volumeIndex, nodeIndex;
	decodeHandle(volumeHandle, &volumeType, &volumeIndex, &nodeIndex);
	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeIndex);

	generateFloor(volume, lowerLayerHeight, lowerLayerMaterial, upperLayerHeight, upperLayerMaterial);

	CLOSE_C_INTERFACE
}
