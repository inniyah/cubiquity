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

// The single global instance of the above class.
EntryAndExitPoints gEntryAndExitPoints;

std::vector<ColouredCubesVolume*> gColouredCubesVolumes;

uint32_t encodeNodeHandle(uint32_t volumeHandle, uint32_t nodeHandle)
{
	if((volumeHandle >= 8) || (nodeHandle >= 65536))
	{
		return -1;
	}

	volumeHandle = volumeHandle << 28;
	return volumeHandle | nodeHandle;
}

void decodeNodeHandle(uint32_t nodeHandle, uint32_t* volumePart, uint32_t* nodePart)
{
	// Validation needed?
	*volumePart = nodeHandle >> 28;
	*nodePart = nodeHandle & 0x0000FFFF;
}

ColouredCubesVolume* getVolumeFromHandle(uint32_t volumeHandle)
{
	return gColouredCubesVolumes[volumeHandle];
}

OctreeNode<Colour>* getNodeFromHandle(uint32_t nodeHandle)
{
	uint32_t volumePart;
	uint32_t nodePart;
	decodeNodeHandle(nodeHandle, &volumePart, &nodePart);

	ColouredCubesVolume* volume = gColouredCubesVolumes[volumePart];
	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(nodePart);
	return node;
}

////////////////////////////////////////////////////////////////////////////////
// Volume functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API void cuNewColouredCubesVolume(uint32_t* result, int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, uint32_t blockSize, uint32_t baseNodeSize)
{
	//logMessage("In cuNewColouredCubesVolume");

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
}

CUBIQUITYC_API void cuNewColouredCubesVolumeFromVolDat(uint32_t* result, const char* volDatToImport, uint32_t blockSize, uint32_t baseNodeSize)
{
	//setLogVerbosity(LogLevels::Info);

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
}

CUBIQUITYC_API void cuUpdateVolume(uint32_t volumeHandle)
{
	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);

	volume->update(Vector3F(0.0f, 0.0f, 0.0f), 0);
}

CUBIQUITYC_API void cuDeleteColouredCubesVolume(uint32_t volumeHandle)
{
	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	delete volume;

	// In the future we could consider reusing this slot as we can detect that it set to zero.
	gColouredCubesVolumes[volumeHandle] = 0;
}

CUBIQUITYC_API void cuGetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t* alpha)
{
	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	Colour& colour = volume->getVoxelAt(x, y, z);
	*red = colour.getRed();
	*green = colour.getGreen();
	*blue = colour.getBlue();
	*alpha = colour.getAlpha();
}

CUBIQUITYC_API void cuSetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
	try
	{
		ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
		volume->setVoxelAt(x, y, z, Colour(red, green, blue, alpha), UpdatePriorities::Immediate);
	}
	catch (const std::exception& ex)
	{
		// Should log it!
	}
	catch (...)
	{
		// Should log it!
	}
}

////////////////////////////////////////////////////////////////////////////////
// Octree functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API void cuHasRootOctreeNode(uint32_t* result, uint32_t volumeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];
	OctreeNode<Colour>* node = volume->getRootOctreeNode();
	if(node)
	{
		*result = 1;
	}
	else
	{
		*result = 0;
	}
}

CUBIQUITYC_API void cuGetRootOctreeNode(uint32_t* result, uint32_t volumeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];
	OctreeNode<Colour>* node = volume->getRootOctreeNode();
	uint32_t nodeHandle = node->mSelf;

	uint32_t combinedHandle = encodeNodeHandle(volumeHandle, nodeHandle);

	*result = combinedHandle;
}

CUBIQUITYC_API void cuHasChildNode(uint32_t* result, uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);
	OctreeNode<Colour>* child = node->getChildNode(childX, childY, childZ);
	if(child)
	{
		*result =  1;
	}
	else
	{
		*result = 0;
	}
}

CUBIQUITYC_API void cuGetChildNode(uint32_t* result, uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);
	OctreeNode<Colour>* child = node->getChildNode(childX, childY, childZ);
	if(child)
	{
		*result =  child->mSelf;
	}
	else
	{
		*result = 1000000000;
	}
}

CUBIQUITYC_API void cuNodeHasMesh(uint32_t* result, uint32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);
	*result = (node->mPolyVoxMesh != 0) ? 1 : 0;
}

CUBIQUITYC_API void cuGetNodePosition(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);
	const Vector3I& lowerCorner = node->mRegion.getLowerCorner();
	*x = lowerCorner.getX();
	*y = lowerCorner.getY();
	*z = lowerCorner.getZ();
}

CUBIQUITYC_API void cuGetMeshLastUpdated(uint32_t* result, uint32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);
	*result = node->mMeshLastUpdated;
}

////////////////////////////////////////////////////////////////////////////////
// Mesh functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API void cuGetNoOfVertices(uint32_t* result, uint32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfVertices();
}

CUBIQUITYC_API void cuGetNoOfIndices(uint32_t* result, uint32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	*result = polyVoxMesh->getNoOfIndices();
}

CUBIQUITYC_API float* cuGetVertices(uint32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< typename VoxelTraits<Colour>::VertexType >& vertexVector = polyVoxMesh->getVertices();

	const VoxelTraits<Colour>::VertexType* vertexPointer = &(vertexVector[0]);

	const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);

	float* floatPointer = const_cast<float*>(constFloatPointer);

	return floatPointer;
}

CUBIQUITYC_API uint32_t* cuGetIndices(uint32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< unsigned int >& indexVector = polyVoxMesh->getIndices();
	const unsigned int* constUIntPointer = &(indexVector[0]);

	unsigned int* uintPointer = const_cast<unsigned int*>(constUIntPointer);

	return uintPointer;
}

////////////////////////////////////////////////////////////////////////////////
// Clock functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API uint32_t cuGetCurrentTime(void)
{
	return Clock::getTimestamp();
}

////////////////////////////////////////////////////////////////////////////////
// Raycasting functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuPickVoxel(uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ)
{
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
		return 1;
	}

	return -1;
}