// CubiquityC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CubiquityC.h"

#include "ColouredCubesVolume.h"
#include "Logging.h"
#include "OctreeNode.h"
#include "Raycasting.h"
#include "SmoothTerrainVolumeEditor.h"
#include "VolumeSerialisation.h"

#include <vector>

#define OPEN_C_INTERFACE \
	try \
	{

#define CLOSE_C_INTERFACE \
	return 1; \
	} \
	catch (const std::exception& ex) \
	{ \
		logError() << "An exception has reached the C interface and been suppressed."; \
		logError() << "\tType: std::exception"; \
		logError() << "\tMessage: \"" << ex.what() << "\""; \
		return -1; \
	} \
	catch (...) \
	{ \
		logError() << "An unrecognised exception has reached the C interface and been suppressed."; \
		return -1; \
	} \

using namespace Cubiquity;

// This class (via it's single global instance) allows code to be executed when the library is loaded and unloaded.
// I do have some concerns about how robust this is - in particular see here: http://stackoverflow.com/a/1229542
class EntryAndExitPoints
{
public:
	EntryAndExitPoints()
	{
		// Note that we can't actually write to the log from this entry point
		// code as the global Boost.Log source might not have been created yet.
		setLogVerbosity(LogLevels::Trace);
	}

	~EntryAndExitPoints()
	{
	}
};

const int TotalHandleBits = 32;
const int VolumeHandleBits = 3;
const int MaxVolumeHandle = (0x01 << VolumeHandleBits) - 1;
const int VolumeHandleShift = TotalHandleBits - VolumeHandleBits - 1;

const int NodeHandleBits = 16; // Set this properly later.
const int NodeHandleMask = (0x01 << NodeHandleBits) - 1;
const int MaxNodeHandle = (0x01 << NodeHandleBits) - 1;

// The single global instance of the above class.
EntryAndExitPoints gEntryAndExitPoints;

std::vector<ColouredCubesVolumeImpl*> gColouredCubesVolumes;
std::vector<SmoothTerrainVolumeImpl*> gSmoothTerrainVolumes;

void validateVolumeHandle(uint32_t volumeHandle)
{
	if(volumeHandle > MaxVolumeHandle)
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' exceeds the maximum permitted value of '" << MaxVolumeHandle << "'";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	if(volumeHandle >= gColouredCubesVolumes.size())
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' is outside volume array bounds";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	if(gColouredCubesVolumes[volumeHandle] == 0) 
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' is valid but the corresponding volume pointer is null";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}
}

void validateVolumeHandleMC(uint32_t volumeHandle)
{
	if(volumeHandle > MaxVolumeHandle)
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' exceeds the maximum permitted value of '" << MaxVolumeHandle << "'";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	if(volumeHandle >= gSmoothTerrainVolumes.size())
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' is outside volume array bounds";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	if(gSmoothTerrainVolumes[volumeHandle] == 0) 
	{
		std::stringstream ss;
		ss << "Volume handle'" << volumeHandle << "' is valid but the corresponding volume pointer is null";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}
}

ColouredCubesVolumeImpl* getVolumeFromHandle(uint32_t volumeHandle)
{
	validateVolumeHandle(volumeHandle);
	return gColouredCubesVolumes[volumeHandle];
}

SmoothTerrainVolumeImpl* getVolumeFromHandleMC(uint32_t volumeHandle)
{
	validateVolumeHandleMC(volumeHandle);
	return gSmoothTerrainVolumes[volumeHandle];
}

void validateDecodedNodeHandle(uint32_t volumeHandle, uint32_t decodedNodeHandle)
{
	if(decodedNodeHandle > MaxNodeHandle)
	{
		std::stringstream ss;
		ss << "Decoded Node handle'" << decodedNodeHandle << "' exceeds the maximum permitted value of '" << MaxNodeHandle << "'";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	// Get the volume (also validates the volume handle)
	ColouredCubesVolumeImpl* volume = getVolumeFromHandle(volumeHandle);

	// Check the node really exists in the volume
	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
	if(!node)
	{
		std::stringstream ss;
		ss << "Decoded Node handle'" << decodedNodeHandle << "' is does not reference a valid node.";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}
}

void validateDecodedNodeHandleMC(uint32_t volumeHandle, uint32_t decodedNodeHandle)
{
	if(decodedNodeHandle > MaxNodeHandle)
	{
		std::stringstream ss;
		ss << "Decoded Node handle'" << decodedNodeHandle << "' exceeds the maximum permitted value of '" << MaxNodeHandle << "'";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}

	// Get the volume (also validates the volume handle)
	SmoothTerrainVolumeImpl* volume = getVolumeFromHandleMC(volumeHandle);

	// Check the node really exists in the volume
	OctreeNode<MultiMaterial>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
	if(!node)
	{
		std::stringstream ss;
		ss << "Decoded Node handle'" << decodedNodeHandle << "' is does not reference a valid node.";
		POLYVOX_THROW(std::invalid_argument, ss.str());
	}
}

uint32_t encodeNodeHandle(uint32_t volumeHandle, uint32_t decodedNodeHandle)
{
	validateDecodedNodeHandle(volumeHandle, decodedNodeHandle);

	uint32_t shiftedVolumeHandle = volumeHandle << VolumeHandleShift;
	return shiftedVolumeHandle | decodedNodeHandle;
}

uint32_t encodeNodeHandleMC(uint32_t volumeHandle, uint32_t decodedNodeHandle)
{
	validateDecodedNodeHandleMC(volumeHandle, decodedNodeHandle);

	uint32_t shiftedVolumeHandle = volumeHandle << VolumeHandleShift;
	return shiftedVolumeHandle | decodedNodeHandle;
}

void decodeNodeHandle(uint32_t encodedNodeHandle, uint32_t* volumeHandle, uint32_t* decodedNodeHandle)
{
	*volumeHandle = encodedNodeHandle >> VolumeHandleShift;
	*decodedNodeHandle = encodedNodeHandle & NodeHandleMask;

	validateDecodedNodeHandle(*volumeHandle, *decodedNodeHandle);
}

void decodeNodeHandleMC(uint32_t encodedNodeHandle, uint32_t* volumeHandle, uint32_t* decodedNodeHandle)
{
	*volumeHandle = encodedNodeHandle >> VolumeHandleShift;
	*decodedNodeHandle = encodedNodeHandle & NodeHandleMask;

	validateDecodedNodeHandleMC(*volumeHandle, *decodedNodeHandle);
}

OctreeNode<Colour>* getNodeFromEncodedHandle(uint32_t encodedNodeHandle)
{
	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandle(encodedNodeHandle, &volumeHandle, &decodedNodeHandle);

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
	return node;
}

OctreeNode<MultiMaterial>* getNodeFromEncodedHandleMC(uint32_t encodedNodeHandle)
{
	uint32_t volumeHandle;
	uint32_t decodedNodeHandle;
	decodeNodeHandleMC(encodedNodeHandle, &volumeHandle, &decodedNodeHandle);

	SmoothTerrainVolume* volume = getVolumeFromHandleMC(volumeHandle);
	OctreeNode<MultiMaterial>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
	return node;
}

////////////////////////////////////////////////////////////////////////////////
// Volume functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuNewColouredCubesVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, const char* pageFolder, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColouredCubesVolumeImpl* volume = new ColouredCubesVolumeImpl(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), pageFolder, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.

	// Replace an existing entry if it has been deleted.
	bool foundEmptySlot = false;
	for(uint32_t ct = 0; ct < gColouredCubesVolumes.size(); ct++)
	{
		if(gColouredCubesVolumes[ct] == 0)
		{
			gColouredCubesVolumes[ct] = volume;
			*result =  ct;
			foundEmptySlot = true;
			break;
		}
	}

	//Otherwise append a new entry.
	if(!foundEmptySlot)
	{
		gColouredCubesVolumes.push_back(volume);
		*result = gColouredCubesVolumes.size() - 1;
	}

	logTrace() << "Created new colored cubes volume in slot " << *result;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNewColouredCubesVolumeFromVolDat(const char* volDatToImport, const char* pageFolder, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColouredCubesVolumeImpl* volume = importVolDat< ColouredCubesVolumeImpl >(volDatToImport, pageFolder, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.
	
	// Replace an existing entry if it has been deleted.
	bool foundEmptySlot = false;
	for(uint32_t ct = 0; ct < gColouredCubesVolumes.size(); ct++)
	{
		if(gColouredCubesVolumes[ct] == 0)
		{
			gColouredCubesVolumes[ct] = volume;
			*result =  ct;
			foundEmptySlot = true;
			break;
		}
	}

	//Otherwise append a new entry.
	if(!foundEmptySlot)
	{
		gColouredCubesVolumes.push_back(volume);
		*result = gColouredCubesVolumes.size() - 1;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNewColouredCubesVolumeFromHeightmap(const char* heightmapFileName, const char* colormapFileName, const char* pageFolder, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColouredCubesVolumeImpl* volume = importHeightmap(heightmapFileName, colormapFileName, pageFolder, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.
	
	// Replace an existing entry if it has been deleted.
	bool foundEmptySlot = false;
	for(uint32_t ct = 0; ct < gColouredCubesVolumes.size(); ct++)
	{
		if(gColouredCubesVolumes[ct] == 0)
		{
			gColouredCubesVolumes[ct] = volume;
			*result =  ct;
			foundEmptySlot = true;
			break;
		}
	}

	//Otherwise append a new entry.
	if(!foundEmptySlot)
	{
		gColouredCubesVolumes.push_back(volume);
		*result = gColouredCubesVolumes.size() - 1;
	}

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuUpdateVolume(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);

	volume->update(Vector3F(0.0f, 0.0f, 0.0f), 0);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuDeleteColouredCubesVolume(uint32_t volumeHandle)
{
	logTrace() << "In cuDeleteColouredCubesVolume() - deleting volume handle '" << volumeHandle << "'";

	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	delete volume;

	// In the future we could consider reusing this slot as we can detect that it set to zero.
	gColouredCubesVolumes[volumeHandle] = 0;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetEnclosingRegion(uint32_t volumeHandle, int32_t* lowerX, int32_t* lowerY, int32_t* lowerZ, int32_t* upperX, int32_t* upperY, int32_t* upperZ)
{
	OPEN_C_INTERFACE

	ColouredCubesVolumeImpl* volume = getVolumeFromHandle(volumeHandle);
	const Region& region = volume->getEnclosingRegion();
	*lowerX = region.getLowerCorner().getX();
	*lowerY = region.getLowerCorner().getY();
	*lowerZ = region.getLowerCorner().getZ();
	*upperX = region.getUpperCorner().getX();
	*upperY = region.getUpperCorner().getY();
	*upperZ = region.getUpperCorner().getZ();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t* alpha)
{
	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	Colour& colour = volume->getVoxelAt(x, y, z);
	*red = colour.getRed();
	*green = colour.getGreen();
	*blue = colour.getBlue();
	*alpha = colour.getAlpha();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuSetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	volume->setVoxelAt(x, y, z, Colour(red, green, blue, alpha), UpdatePriorities::Immediate);
	
	CLOSE_C_INTERFACE
}

//--------------------------------------------------------------------------------

CUBIQUITYC_API int32_t cuNewSmoothTerrainVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, const char* pageFolder, uint32_t baseNodeSize, uint32_t createFloor, uint32_t floorDepth, uint32_t* result)
{
	OPEN_C_INTERFACE

	SmoothTerrainVolumeImpl* volume = dynamic_cast<SmoothTerrainVolumeImpl*>(createSmoothTerrainVolume(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), pageFolder, baseNodeSize, createFloor==1, floorDepth));
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.

	// Replace an existing entry if it has been deleted.
	bool foundEmptySlot = false;
	for(uint32_t ct = 0; ct < gSmoothTerrainVolumes.size(); ct++)
	{
		if(gSmoothTerrainVolumes[ct] == 0)
		{
			gSmoothTerrainVolumes[ct] = volume;
			*result =  ct;
			foundEmptySlot = true;
			break;
		}
	}

	//Otherwise append a new entry.
	if(!foundEmptySlot)
	{
		gSmoothTerrainVolumes.push_back(volume);
		*result = gSmoothTerrainVolumes.size() - 1;
	}

	logTrace() << "Creatd new smooth volume in slot " << *result;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuUpdateVolumeMC(uint32_t volumeHandle)
{
	OPEN_C_INTERFACE

	SmoothTerrainVolume* volume = getVolumeFromHandleMC(volumeHandle);

	volume->update(Vector3F(0.0f, 0.0f, 0.0f), 0);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuDeleteSmoothTerrainVolume(uint32_t volumeHandle)
{
	logTrace() << "In cuDeleteSmoothTerrainVolume() - deleting volume handle '" << volumeHandle << "'";

	OPEN_C_INTERFACE

	SmoothTerrainVolume* volume = getVolumeFromHandleMC(volumeHandle);
	delete volume;

	// In the future we could consider reusing this slot as we can detect that it set to zero.
	gSmoothTerrainVolumes[volumeHandle] = 0;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetEnclosingRegionMC(uint32_t volumeHandle, int32_t* lowerX, int32_t* lowerY, int32_t* lowerZ, int32_t* upperX, int32_t* upperY, int32_t* upperZ)
{
	OPEN_C_INTERFACE

	SmoothTerrainVolumeImpl* volume = getVolumeFromHandleMC(volumeHandle);
	const Region& region = volume->getEnclosingRegion();
	*lowerX = region.getLowerCorner().getX();
	*lowerY = region.getLowerCorner().getY();
	*lowerZ = region.getLowerCorner().getZ();
	*upperX = region.getUpperCorner().getX();
	*upperY = region.getUpperCorner().getY();
	*upperZ = region.getUpperCorner().getZ();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVoxelMC(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint32_t index, uint8_t* value)
{
	OPEN_C_INTERFACE

	SmoothTerrainVolume* volume = getVolumeFromHandleMC(volumeHandle);
	MultiMaterial& material = volume->getVoxelAt(x, y, z);
	*value = material.getMaterial(index);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuSetVoxelMC(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint32_t index, uint8_t value)
{
	OPEN_C_INTERFACE

	SmoothTerrainVolume* volume = getVolumeFromHandleMC(volumeHandle);
	MultiMaterial& material = volume->getVoxelAt(x, y, z);
	material.setMaterial(index, value);
	volume->setVoxelAt(x, y, z, material, UpdatePriorities::Immediate);
	
	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Octree functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuHasRootOctreeNode(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	OctreeNode<Colour>* node = volume->getRootOctreeNode();
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

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	OctreeNode<Colour>* node = volume->getRootOctreeNode();

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

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);
	OctreeNode<Colour>* child = node->getChildNode(childX, childY, childZ);
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

CUBIQUITYC_API int32_t cuGetChildNode(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);
	OctreeNode<Colour>* child = node->getChildNode(childX, childY, childZ);

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

CUBIQUITYC_API int32_t cuNodeHasMesh(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);
	*result = (node->mPolyVoxMesh != 0) ? 1 : 0;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNodePosition(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z)
{
	OPEN_C_INTERFACE

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);
	const Vector3I& lowerCorner = node->mRegion.getLowerCorner();
	*x = lowerCorner.getX();
	*y = lowerCorner.getY();
	*z = lowerCorner.getZ();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetMeshLastUpdated(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);
	*result = node->mMeshLastUpdated;

	CLOSE_C_INTERFACE
}

//--------------------------------------------------------------------------------

CUBIQUITYC_API int32_t cuHasRootOctreeNodeMC(uint32_t volumeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	SmoothTerrainVolume* volume = getVolumeFromHandleMC(volumeHandle);
	OctreeNode<MultiMaterial>* node = volume->getRootOctreeNode();
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

	SmoothTerrainVolume* volume = getVolumeFromHandleMC(volumeHandle);
	OctreeNode<MultiMaterial>* node = volume->getRootOctreeNode();

	if(!node)
	{
		POLYVOX_THROW(PolyVox::invalid_operation, "No root node exists! Please check this with cuHasRootOctreeNode() first");
	}

	uint32_t decodedNodeHandle = node->mSelf;

	*result = encodeNodeHandleMC(volumeHandle, decodedNodeHandle);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuHasChildNodeMC(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);
	OctreeNode<MultiMaterial>* child = node->getChildNode(childX, childY, childZ);
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

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);
	OctreeNode<MultiMaterial>* child = node->getChildNode(childX, childY, childZ);

	if(!node)
	{
		POLYVOX_THROW(PolyVox::invalid_operation, "The specified child node does not exist! Please check this with cuHasChildNode() first");
	}

	uint32_t decodedNodeHandle = child->mSelf;

	uint32_t volumeHandle;
	uint32_t dummy;
	decodeNodeHandleMC(nodeHandle, &volumeHandle, &dummy);

	*result = encodeNodeHandleMC(volumeHandle, decodedNodeHandle);

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNodeHasMeshMC(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);
	*result = (node->mPolyVoxMesh != 0) ? 1 : 0;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNodePositionMC(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z)
{
	OPEN_C_INTERFACE

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);
	const Vector3I& lowerCorner = node->mRegion.getLowerCorner();
	*x = lowerCorner.getX();
	*y = lowerCorner.getY();
	*z = lowerCorner.getZ();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetMeshLastUpdatedMC(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);
	*result = node->mMeshLastUpdated;

	CLOSE_C_INTERFACE
}

////////////////////////////////////////////////////////////////////////////////
// Mesh functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuGetNoOfVertices(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfVertices();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNoOfIndices(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfIndices();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVertices(uint32_t nodeHandle, float** result)
{
	OPEN_C_INTERFACE

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< typename VoxelTraits<Colour>::VertexType >& vertexVector = polyVoxMesh->getVertices();

	const VoxelTraits<Colour>::VertexType* vertexPointer = &(vertexVector[0]);

	const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);

	float* floatPointer = const_cast<float*>(constFloatPointer);

	*result = floatPointer;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetIndices(uint32_t nodeHandle, uint32_t** result)
{
	OPEN_C_INTERFACE

	OctreeNode<Colour>* node = getNodeFromEncodedHandle(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< unsigned int >& indexVector = polyVoxMesh->getIndices();
	const unsigned int* constUIntPointer = &(indexVector[0]);

	unsigned int* uintPointer = const_cast<unsigned int*>(constUIntPointer);

	*result = uintPointer;

	CLOSE_C_INTERFACE
}

//--------------------------------------------------------------------------------

CUBIQUITYC_API int32_t cuGetNoOfVerticesMC(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<MultiMaterial>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfVertices();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetNoOfIndicesMC(uint32_t nodeHandle, uint32_t* result)
{
	OPEN_C_INTERFACE

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<MultiMaterial>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfIndices();

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetVerticesMC(uint32_t nodeHandle, float** result)
{
	OPEN_C_INTERFACE

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<MultiMaterial>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< typename VoxelTraits<MultiMaterial>::VertexType >& vertexVector = polyVoxMesh->getVertices();

	const VoxelTraits<MultiMaterial>::VertexType* vertexPointer = &(vertexVector[0]);

	const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);

	float* floatPointer = const_cast<float*>(constFloatPointer);

	*result = floatPointer;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuGetIndicesMC(uint32_t nodeHandle, uint32_t** result)
{
	OPEN_C_INTERFACE

	OctreeNode<MultiMaterial>* node = getNodeFromEncodedHandleMC(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<MultiMaterial>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< unsigned int >& indexVector = polyVoxMesh->getIndices();
	const unsigned int* constUIntPointer = &(indexVector[0]);

	unsigned int* uintPointer = const_cast<unsigned int*>(constUIntPointer);

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

	ColouredCubesVolumeImpl* volume = getVolumeFromHandle(volumeHandle);

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

	ColouredCubesVolumeImpl* volume = getVolumeFromHandle(volumeHandle);

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

	SmoothTerrainVolumeImpl* volume = getVolumeFromHandleMC(volumeHandle);

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

CUBIQUITYC_API int32_t cuSculptSmoothTerrainVolume(uint32_t volumeHandle, float centerX, float centerY, float centerZ, float brushRadius, float speed)
{
	OPEN_C_INTERFACE

	SmoothTerrainVolumeImpl* volume = getVolumeFromHandleMC(volumeHandle);

	sculptSmoothTerrainVolume(volume, Vector3F(centerX, centerY, centerZ), brushRadius, speed);

	CLOSE_C_INTERFACE
}