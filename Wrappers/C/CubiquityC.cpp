// CubiquityC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CubiquityC.h"

#include "ColouredCubesVolume.h"
#include "Logging.h"
#include "OctreeNode.h"
#include "Raycasting.h"
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
		// code as the global Boost.Log source might nothave been created yet.
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

std::vector<ColouredCubesVolume*> gColouredCubesVolumes;

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

ColouredCubesVolume* getVolumeFromHandle(uint32_t volumeHandle)
{
	validateVolumeHandle(volumeHandle);
	return gColouredCubesVolumes[volumeHandle];
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
	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);

	// Check the node really exists in the volume
	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(decodedNodeHandle);
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

void decodeNodeHandle(uint32_t encodedNodeHandle, uint32_t* volumeHandle, uint32_t* decodedNodeHandle)
{
	*volumeHandle = encodedNodeHandle >> VolumeHandleShift;
	*decodedNodeHandle = encodedNodeHandle & NodeHandleMask;

	validateDecodedNodeHandle(*volumeHandle, *decodedNodeHandle);
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

////////////////////////////////////////////////////////////////////////////////
// Volume functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuNewColouredCubesVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, uint32_t blockSize, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = new ColouredCubesVolume(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), blockSize, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.

	//Replace an existing entry if it has been deleted.
	for(uint32_t ct = 0; ct < gColouredCubesVolumes.size(); ct++)
	{
		if(gColouredCubesVolumes[ct] == 0)
		{
			gColouredCubesVolumes[ct] = volume;
			*result = ct;
		}
	}

	//Otherwise append a new entry.
	gColouredCubesVolumes.push_back(volume);
	*result = gColouredCubesVolumes.size() - 1;

	CLOSE_C_INTERFACE
}

CUBIQUITYC_API int32_t cuNewColouredCubesVolumeFromVolDat(const char* volDatToImport, uint32_t blockSize, uint32_t baseNodeSize, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = importVolDat<ColouredCubesVolume>(volDatToImport, blockSize, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.
	//Replace an existing entry if it has been deleted.
	for(uint32_t ct = 0; ct < gColouredCubesVolumes.size(); ct++)
	{
		if(gColouredCubesVolumes[ct] == 0)
		{
			gColouredCubesVolumes[ct] = volume;
			*result =  ct;
		}
	}

	//Otherwise append a new entry.
	gColouredCubesVolumes.push_back(volume);
	*result = gColouredCubesVolumes.size() - 1;

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
	logTrace() << "In cuDeleteColouredCubesVolume()";

	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	delete volume;

	// In the future we could consider reusing this slot as we can detect that it set to zero.
	gColouredCubesVolumes[volumeHandle] = 0;

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
		POLYVOX_THROW(PolyVox::invalid_operation, "No root node exists! Please check this with cuHasChildNode() first");
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
	if(child)
	{
		*result =  child->mSelf;
	}
	else
	{
		*result = 1000000000;
	}

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
CUBIQUITYC_API int32_t cuPickVoxel(uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ, uint32_t* result)
{
	OPEN_C_INTERFACE

	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);

	Vector3F v3dStart(rayStartX, rayStartY, rayStartZ);
	Vector3F v3dDirection(rayDirX, rayDirY, rayDirZ);

	ColouredCubesRaycastTestFunctor raycastTestFunctor;
	PolyVox::RaycastResult myResult = PolyVox::raycastWithDirection(getPolyVoxVolumeFrom(volume), v3dStart, v3dDirection, raycastTestFunctor);
	if(myResult == ::PolyVox::RaycastResults::Interupted)
	{
		//result = gameplay::Vector3(raycastTestFunctor.mLastPos.getX(), raycastTestFunctor.mLastPos.getY(), raycastTestFunctor.mLastPos.getZ());
		*resultX = raycastTestFunctor.mLastPos.getX() + 0.5f;
		*resultY = raycastTestFunctor.mLastPos.getY() + 0.5f;
		*resultZ = raycastTestFunctor.mLastPos.getZ() + 0.5f;
		*result = 1;
	}
	else
	{
		*result = 0;
	}

	CLOSE_C_INTERFACE
}