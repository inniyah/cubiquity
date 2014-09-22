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
#include <stdexcept>
#include <vector>

using namespace std;

const uint32_t CuMajorVersion = 1;
const uint32_t CuMinorVersion = 1;
const uint32_t CuPatchVersion = 2;

namespace VolumeTypes
{
	enum VolumeType
	{
		ColoredCubes,
		Terrain
	};
}
typedef VolumeTypes::VolumeType VolumeType;

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

// This class (via it's single global instance) allows code to be executed when the library is loaded and unloaded.
// I do have some concerns about how robust this is - in particular see here: http://stackoverflow.com/a/1229542
class EntryAndExitPoints
{
public:
	EntryAndExitPoints()
		:mFileLogger()
	{
		PolyVox::setLogger(&mFileLogger);
	}

	~EntryAndExitPoints()
	{
		PolyVox::setLogger(0);
	}

public:
	FileLogger mFileLogger;
};

const int TotalHandleBits = 32;
const int VolumeHandleBits = 8;
const int MaxVolumeHandle = (0x01 << VolumeHandleBits) - 1;
const int VolumeHandleShift = TotalHandleBits - VolumeHandleBits - 1;

const int NodeHandleBits = 16; // Set this properly later.
const int NodeHandleMask = (0x01 << NodeHandleBits) - 1;
const int MaxNodeHandle = (0x01 << NodeHandleBits) - 1;

// The single global instance of the above class.
EntryAndExitPoints gEntryAndExitPoints;

/*std::vector<ColoredCubesVolume*> gColoredCubesVolumes;
std::vector<TerrainVolume*> gTerrainVolumes;*/
std::vector<void*> gVolumes;
std::vector<VolumeType> gVolumeTypes;

void validateVolumeHandle(uint32_t volumeHandle)
{
	if(volumeHandle > MaxVolumeHandle)
	{
		std::stringstream ss;
		ss << "Volume handle' " << volumeHandle << "' exceeds the maximum permitted value of '" << MaxVolumeHandle << "'";
		std::string errorString(ss.str());
		POLYVOX_THROW(std::invalid_argument, errorString);
	}

	if(volumeHandle >= gVolumes.size())
	{
		std::stringstream ss;
		ss << "Volume handle' " << volumeHandle << "' is outside volume array bounds";
		std::string errorString(ss.str());
		POLYVOX_THROW(std::invalid_argument, errorString);
	}

	if(gVolumes[volumeHandle] == 0) 
	{
		std::stringstream ss;
		ss << "Volume handle' " << volumeHandle << "' is valid but the corresponding volume pointer is null";
		std::string errorString(ss.str());
		POLYVOX_THROW(std::invalid_argument, errorString);
	}
}

/*void validateVolumeHandleMC(uint32_t volumeHandle)
{
	if(volumeHandle > MaxVolumeHandle)
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' exceeds the maximum permitted value of '" << MaxVolumeHandle << "'";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	if(volumeHandle >= gTerrainVolumes.size())
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' is outside volume array bounds";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	if(gTerrainVolumes[volumeHandle] == 0) 
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' is valid but the corresponding volume pointer is null";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}
}*/

void* getVolumeFromHandle(uint32_t volumeHandle)
{
	validateVolumeHandle(volumeHandle);
	return gVolumes[volumeHandle];
}

ColoredCubesVolume* getColoredCubesVolumeFromHandle(uint32_t volumeHandle)
{
	validateVolumeHandle(volumeHandle);
	POLYVOX_THROW_IF(gVolumeTypes[volumeHandle] != VolumeTypes::ColoredCubes, std::invalid_argument, "Wrong volume type");
	ColoredCubesVolume* volume = reinterpret_cast<ColoredCubesVolume*>(gVolumes[volumeHandle]);
	return volume;
}

TerrainVolume* getTerrainVolumeFromHandle(uint32_t volumeHandle)
{
	validateVolumeHandle(volumeHandle);
	POLYVOX_THROW_IF(gVolumeTypes[volumeHandle] != VolumeTypes::Terrain, std::invalid_argument, "Wrong volume type");
	TerrainVolume* volume = reinterpret_cast<TerrainVolume*>(gVolumes[volumeHandle]);
	return volume;
}

/*TerrainVolume* getVolumeFromHandleMC(uint32_t volumeHandle)
{
	validateVolumeHandleMC(volumeHandle);
	return gTerrainVolumes[volumeHandle];
}*/

void validateDecodedNodeHandle(uint32_t volumeHandle, uint32_t decodedNodeHandle)
{
	if(decodedNodeHandle > MaxNodeHandle)
	{
		std::stringstream ss;
		ss << "Decoded Node handle'" << decodedNodeHandle << "' exceeds the maximum permitted value of '" << MaxNodeHandle << "'";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	if(gVolumeTypes[volumeHandle] == VolumeTypes::ColoredCubes)
	{
		// Get the volume (also validates the volume handle)
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);

		// Check the node really exists in the volume
		OctreeNode<Color>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
		if(!node)
		{
			std::stringstream ss;
			ss << "Decoded Node handle'" << decodedNodeHandle << "' is does not reference a valid node.";
			POLYVOX_THROW(std::invalid_argument, ss.str());
		}
	}
	else
	{
		// Get the volume (also validates the volume handle)
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

		// Check the node really exists in the volume
		OctreeNode<MaterialSet>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
		if(!node)
		{
			std::stringstream ss;
			ss << "Decoded Node handle'" << decodedNodeHandle << "' is does not reference a valid node.";
			POLYVOX_THROW(std::invalid_argument, ss.str());
		}
	}
}

/*void validateDecodedNodeHandleMC(uint32_t volumeHandle, uint32_t decodedNodeHandle)
{
	if(decodedNodeHandle > MaxNodeHandle)
	{
		std::stringstream ss;
		ss << "Decoded Node handle'" << decodedNodeHandle << "' exceeds the maximum permitted value of '" << MaxNodeHandle << "'";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	// Get the volume (also validates the volume handle)
	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

	// Check the node really exists in the volume
	OctreeNode<MaterialSet>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
	if(!node)
	{
		std::stringstream ss;
		ss << "Decoded Node handle'" << decodedNodeHandle << "' is does not reference a valid node.";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}
}*/

uint32_t encodeNodeHandle(uint32_t volumeHandle, uint32_t decodedNodeHandle)
{
	validateDecodedNodeHandle(volumeHandle, decodedNodeHandle);

	uint32_t shiftedVolumeHandle = volumeHandle << VolumeHandleShift;
	return shiftedVolumeHandle | decodedNodeHandle;
}

/*uint32_t encodeNodeHandleMC(uint32_t volumeHandle, uint32_t decodedNodeHandle)
{
	validateDecodedNodeHandleMC(volumeHandle, decodedNodeHandle);

	uint32_t shiftedVolumeHandle = volumeHandle << VolumeHandleShift;
	return shiftedVolumeHandle | decodedNodeHandle;
}*/

void decodeNodeHandle(uint32_t encodedNodeHandle, uint32_t* volumeHandle, uint32_t* decodedNodeHandle)
{
	*volumeHandle = encodedNodeHandle >> VolumeHandleShift;
	*decodedNodeHandle = encodedNodeHandle & NodeHandleMask;

	validateDecodedNodeHandle(*volumeHandle, *decodedNodeHandle);
}

/*void decodeNodeHandleMC(uint32_t encodedNodeHandle, uint32_t* volumeHandle, uint32_t* decodedNodeHandle)
{
	*volumeHandle = encodedNodeHandle >> VolumeHandleShift;
	*decodedNodeHandle = encodedNodeHandle & NodeHandleMask;

	validateDecodedNodeHandleMC(*volumeHandle, *decodedNodeHandle);
}*/

void* getNodeFromEncodedHandle(uint32_t encodedNodeHandle)
{
	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandle(encodedNodeHandle, &volumeHandle, &decodedNodeHandle);

	if(gVolumeTypes[volumeHandle] == VolumeTypes::ColoredCubes)
	{
		ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);
		OctreeNode<Color>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
		return node;
	}
	else
	{
		TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
		OctreeNode<MaterialSet>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
		return node;
	}
}

/*OctreeNode<MaterialSet>* getNodeFromEncodedHandleMC(uint32_t encodedNodeHandle)
{
	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandleMC(encodedNodeHandle, &volumeHandle, &decodedNodeHandle);

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
	OctreeNode<MaterialSet>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
	return node;
}*/

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
	for(uint32_t ct = 0; ct < gVolumes.size(); ct++)
	{
		if(gVolumes[ct] == 0)
		{
			gVolumes[ct] = volume;
			gVolumeTypes[ct] = VolumeTypes::ColoredCubes;
			*result =  ct;
			foundEmptySlot = true;
			break;
		}
	}

	//Otherwise append a new entry.
	if(!foundEmptySlot)
	{
		gVolumes.push_back(volume);
		gVolumeTypes.push_back(VolumeTypes::ColoredCubes);
		*result = gVolumes.size() - 1;
	}

	POLYVOX_LOG_DEBUG("Created new colored cubes volume in slot " << *result);

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
	for(uint32_t ct = 0; ct < gVolumes.size(); ct++)
	{
		if(gVolumes[ct] == 0)
		{
			gVolumes[ct] = volume;
			gVolumeTypes[ct] = VolumeTypes::ColoredCubes;
			*result =  ct;
			foundEmptySlot = true;
			break;
		}
	}

	//Otherwise append a new entry.
	if(!foundEmptySlot)
	{
		gVolumes.push_back(volume);
		gVolumeTypes.push_back(VolumeTypes::ColoredCubes);
		*result = gVolumes.size() - 1;
	}

	POLYVOX_LOG_DEBUG("Created new colored cubes volume in slot " << *result);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuUpdateVolume(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);

	volume->update(Vector3F(0.0f, 0.0f, 0.0f), 0);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuDeleteColoredCubesVolume(uint32_t volumeHandle)
{
	POLYVOX_LOG_DEBUG("In cuDeleteColoredCubesVolume() - deleting volume handle '" << volumeHandle << "'");

	OPEN_C_INTERFACE

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);
	delete volume;

	// In the future we could consider reusing this slot as we can detect that it set to zero.
	gVolumes[volumeHandle] = 0;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetEnclosingRegion(uint32_t volumeHandle, int32_t* lowerX, int32_t* lowerY, int32_t* lowerZ, int32_t* upperX, int32_t* upperY, int32_t* upperZ)
{
	OPEN_C_INTERFACE

	if(gVolumeTypes[volumeHandle] == VolumeTypes::ColoredCubes)
	{
		ColoredCubesVolume* coloredCubesVolume = getColoredCubesVolumeFromHandle(volumeHandle);
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
		TerrainVolume* terrainVolume = getTerrainVolumeFromHandle(volumeHandle);
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

CUBIQUITYC_API int32_t cuGetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, CuColor* color)
{
	OPEN_C_INTERFACE

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);
	Color temp = volume->getVoxelAt(x, y, z);

	CuColor* ptr = (CuColor*)&temp;

	*color = *ptr;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuSetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, CuColor color)
{
	OPEN_C_INTERFACE

	Color* pColor = (Color*)&color;

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);
	volume->setVoxelAt(x, y, z, *pColor, UpdatePriorities::Immediate);
	
	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuAcceptOverrideChunks(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);
	volume->acceptOverrideChunks();
	
	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuDiscardOverrideChunks(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);
	volume->discardOverrideChunks();
	
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
	for(uint32_t ct = 0; ct < gVolumes.size(); ct++)
	{
		if(gVolumes[ct] == 0)
		{
			gVolumes[ct] = volume;
			gVolumeTypes[ct] = VolumeTypes::Terrain;
			*result =  ct;
			foundEmptySlot = true;
			break;
		}
	}

	//Otherwise append a new entry.
	if(!foundEmptySlot)
	{
		gVolumes.push_back(volume);
		gVolumeTypes.push_back(VolumeTypes::Terrain);
		*result = gVolumes.size() - 1;
	}

	POLYVOX_LOG_DEBUG("Created new smooth volume in slot " << *result);

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
	for(uint32_t ct = 0; ct < gVolumes.size(); ct++)
	{
		if(gVolumes[ct] == 0)
		{
			gVolumes[ct] = volume;
			gVolumeTypes[ct] = VolumeTypes::Terrain;
			*result =  ct;
			foundEmptySlot = true;
			break;
		}
	}

	//Otherwise append a new entry.
	if(!foundEmptySlot)
	{
		gVolumes.push_back(volume);
		gVolumeTypes.push_back(VolumeTypes::Terrain);
		*result = gVolumes.size() - 1;
	}

	POLYVOX_LOG_DEBUG("Created new smooth volume in slot " << *result);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuUpdateVolumeMC(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

	volume->update(Vector3F(0.0f, 0.0f, 0.0f), 0);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuDeleteTerrainVolume(uint32_t volumeHandle)
{
	POLYVOX_LOG_DEBUG("In cuDeleteTerrainVolume() - deleting volume handle '" << volumeHandle << "'");

	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
	delete volume;

	// In the future we could consider reusing this slot as we can detect that it set to zero.
	gVolumes[volumeHandle] = 0;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVoxelMC(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, CuMaterialSet* materialSet)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
	MaterialSet material = volume->getVoxelAt(x, y, z);

	materialSet->data = material.mWeights.allBits();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuSetVoxelMC(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, CuMaterialSet materialSet)
{
	OPEN_C_INTERFACE

	MaterialSet* pMat = (MaterialSet*)&materialSet;

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
	volume->setVoxelAt(x, y, z, *pMat, UpdatePriorities::Immediate);
	
	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuAcceptOverrideChunksMC(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
	volume->acceptOverrideChunks();
	
	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuDiscardOverrideChunksMC(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
	volume->discardOverrideChunks();
	
	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Octree functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuHasRootOctreeNode(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);
	OctreeNode<Color>* node = volume->getRootOctreeNode();
	if(node)
	{
		*result = 1;
	}
	else
	{
		*result = 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetRootOctreeNode(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);
	OctreeNode<Color>* node = volume->getRootOctreeNode();

	if(!node)
	{
		POLYVOX_THROW(PolyVox::invalid_operation, "No root node exists! Please check this with cuHasRootOctreeNode() first");
	}

	uint32_t decodedNodeHandle = node->mSelf;

	*result = encodeNodeHandle(volumeHandle, decodedNodeHandle);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuHasChildNode(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandle(nodeHandle, &volumeHandle, &decodedNodeHandle);

	if(gVolumeTypes[volumeHandle] == VolumeTypes::ColoredCubes)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));
		OctreeNode<Color>* child = node->getChildNode(childX, childY, childZ);
		if(child)
		{
			*result =  1;
		}
		else
		{
			*result = 0;
		}
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
		OctreeNode<MaterialSet>* child = node->getChildNode(childX, childY, childZ);
		if(child)
		{
			*result =  1;
		}
		else
		{
			*result = 0;
		}
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetChildNode(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandle(nodeHandle, &volumeHandle, &decodedNodeHandle);

	if(gVolumeTypes[volumeHandle] == VolumeTypes::ColoredCubes)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));
		OctreeNode<Color>* child = node->getChildNode(childX, childY, childZ);

		if(!node)
		{
			POLYVOX_THROW(PolyVox::invalid_operation, "The specified child node does not exist! Please check this with cuHasChildNode() first");
		}

		uint32_t decodedNodeHandle = child->mSelf;

		uint32_t volumeHandle;
		uint32_t dummy;
		decodeNodeHandle(nodeHandle, &volumeHandle, &dummy);

		*result = encodeNodeHandle(volumeHandle, decodedNodeHandle);
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
		OctreeNode<MaterialSet>* child = node->getChildNode(childX, childY, childZ);

		if(!node)
		{
			POLYVOX_THROW(PolyVox::invalid_operation, "The specified child node does not exist! Please check this with cuHasChildNode() first");
		}

		uint32_t decodedNodeHandle = child->mSelf;

		uint32_t volumeHandle;
		uint32_t dummy;
		decodeNodeHandle(nodeHandle, &volumeHandle, &dummy);

		*result = encodeNodeHandle(volumeHandle, decodedNodeHandle);
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNodeHasMesh(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandle(nodeHandle, &volumeHandle, &decodedNodeHandle);

	if(gVolumeTypes[volumeHandle] == VolumeTypes::ColoredCubes)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));
		*result = (node->mPolyVoxMesh != 0) ? 1 : 0;
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
		*result = (node->mPolyVoxMesh != 0) ? 1 : 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNodePosition(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z)
{
	OPEN_C_INTERFACE

	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandle(nodeHandle, &volumeHandle, &decodedNodeHandle);

	if(gVolumeTypes[volumeHandle] == VolumeTypes::ColoredCubes)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));
		const Vector3I& lowerCorner = node->mRegion.getLowerCorner();
		*x = lowerCorner.getX();
		*y = lowerCorner.getY();
		*z = lowerCorner.getZ();
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
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

	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandle(nodeHandle, &volumeHandle, &decodedNodeHandle);

	if(gVolumeTypes[volumeHandle] == VolumeTypes::ColoredCubes)
	{
		OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));
		*result = node->mMeshLastUpdated;
	}
	else
	{
		OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
		*result = node->mMeshLastUpdated;
	}

	CLOSE_C_INTERFACE
}

//--------------------------------------------------------------------------------

CUBIQUITYC_API int32_t cuHasRootOctreeNodeMC(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
	OctreeNode<MaterialSet>* node = volume->getRootOctreeNode();
	if(node)
	{
		*result = 1;
	}
	else
	{
		*result = 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetRootOctreeNodeMC(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);
	OctreeNode<MaterialSet>* node = volume->getRootOctreeNode();

	if(!node)
	{
		POLYVOX_THROW(PolyVox::invalid_operation, "No root node exists! Please check this with cuHasRootOctreeNode() first");
	}

	uint32_t decodedNodeHandle = node->mSelf;

	*result = encodeNodeHandle(volumeHandle, decodedNodeHandle);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuHasChildNodeMC(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
	OctreeNode<MaterialSet>* child = node->getChildNode(childX, childY, childZ);
	if(child)
	{
		*result =  1;
	}
	else
	{
		*result = 0;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetChildNodeMC(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
	OctreeNode<MaterialSet>* child = node->getChildNode(childX, childY, childZ);

	if(!node)
	{
		POLYVOX_THROW(PolyVox::invalid_operation, "The specified child node does not exist! Please check this with cuHasChildNode() first");
	}

	uint32_t decodedNodeHandle = child->mSelf;

	uint32_t volumeHandle;
	uint32_t dummy;
	decodeNodeHandle(nodeHandle, &volumeHandle, &dummy);

	*result = encodeNodeHandle(volumeHandle, decodedNodeHandle);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNodeHasMeshMC(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
	*result = (node->mPolyVoxMesh != 0) ? 1 : 0;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNodePositionMC(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
	const Vector3I& lowerCorner = node->mRegion.getLowerCorner();
	*x = lowerCorner.getX();
	*y = lowerCorner.getY();
	*z = lowerCorner.getZ();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetMeshLastUpdatedMC(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));
	*result = node->mMeshLastUpdated;

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Mesh functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuGetNoOfVertices(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));

	const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfVertices();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNoOfIndices(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));

	const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfIndices();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVertices(uint32_t nodeHandle, float** result)
{
	OPEN_C_INTERFACE

	OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));

	const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< typename VoxelTraits<Color>::VertexType >& vertexVector = polyVoxMesh->getVertices();

	const VoxelTraits<Color>::VertexType* vertexPointer = &(vertexVector[0]);

	const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);

	float* floatPointer = const_cast<float*>(constFloatPointer);

	*result = floatPointer;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetIndices(uint32_t nodeHandle, uint16_t** result)
{
	OPEN_C_INTERFACE

	OctreeNode<Color>* node = reinterpret_cast<OctreeNode<Color>*>(getNodeFromEncodedHandle(nodeHandle));

	const ::PolyVox::Mesh< typename VoxelTraits<Color>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< uint16_t >& indexVector = polyVoxMesh->getIndices();
	const uint16_t* constUInt16Pointer = &(indexVector[0]);

	uint16_t* uintPointer = const_cast<uint16_t*>(constUInt16Pointer);

	*result = uintPointer;

	CLOSE_C_INTERFACE
}

//--------------------------------------------------------------------------------

CUBIQUITYC_API int32_t cuGetNoOfVerticesMC(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));

	const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfVertices();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNoOfIndicesMC(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));

	const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfIndices();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVerticesMC(uint32_t nodeHandle, float** result)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));

	const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< typename VoxelTraits<MaterialSet>::VertexType >& vertexVector = polyVoxMesh->getVertices();

	const VoxelTraits<MaterialSet>::VertexType* vertexPointer = &(vertexVector[0]);

	const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);

	float* floatPointer = const_cast<float*>(constFloatPointer);

	*result = floatPointer;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetIndicesMC(uint32_t nodeHandle, uint16_t** result)
{
	OPEN_C_INTERFACE

	OctreeNode<MaterialSet>* node = reinterpret_cast<OctreeNode<MaterialSet>*>(getNodeFromEncodedHandle(nodeHandle));

	const ::PolyVox::Mesh< typename VoxelTraits<MaterialSet>::VertexType, uint16_t >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< uint16_t >& indexVector = polyVoxMesh->getIndices();
	const uint16_t* constUIntPointer = &(indexVector[0]);

	uint16_t* uintPointer = const_cast<uint16_t*>(constUIntPointer);

	*result = uintPointer;

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

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);

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

	ColoredCubesVolume* volume = getColoredCubesVolumeFromHandle(volumeHandle);

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

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

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

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

	sculptTerrainVolume(volume, Vector3F(brushX, brushY, brushZ), Brush(brushInnerRadius, brushOuterRadius, opacity));

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuBlurTerrainVolume(uint32_t volumeHandle, float brushX, float brushY, float brushZ, float brushInnerRadius, float brushOuterRadius, float opacity)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

	blurTerrainVolume(volume, Vector3F(brushX, brushY, brushZ), Brush(brushInnerRadius, brushOuterRadius, opacity));

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuBlurTerrainVolumeRegion(uint32_t volumeHandle, int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

	blurTerrainVolume(volume, Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ));

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuPaintTerrainVolume(uint32_t volumeHandle, float brushX, float brushY, float brushZ, float brushInnerRadius, float brushOuterRadius, float opacity, uint32_t materialIndex)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

	paintTerrainVolume(volume, Vector3F(brushX, brushY, brushZ), Brush(brushInnerRadius, brushOuterRadius, opacity), materialIndex);

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Volume generation functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuGenerateFloor(uint32_t volumeHandle, int32_t lowerLayerHeight, uint32_t lowerLayerMaterial, int32_t upperLayerHeight, uint32_t upperLayerMaterial)
{
	OPEN_C_INTERFACE

	TerrainVolume* volume = getTerrainVolumeFromHandle(volumeHandle);

	generateFloor(volume, lowerLayerHeight, lowerLayerMaterial, upperLayerHeight, upperLayerMaterial);

	CLOSE_C_INTERFACE
}