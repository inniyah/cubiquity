// CubiquityC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CubiquityC.h"

#include "OctreeNode.h"
#include "ColouredCubesVolume.h"
#include "VolumeSerialisation.h"

#include <vector>

using namespace Cubiquity;

std::vector<ColouredCubesVolume*> gColouredCubesVolumes;

ColouredCubesVolume* getVolumeFromHandle(int32_t volumeHandle)
{
	return gColouredCubesVolumes[volumeHandle];
}

OctreeNode<Colour>* getNodeFromHandle(int32_t volumeHandle, int32_t nodeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];
	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(nodeHandle);
	return node;
}

////////////////////////////////////////////////////////////////////////////////
// Volume functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuNewColouredCubesVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, uint32_t blockSize, uint32_t baseNodeSize)
{
	ColouredCubesVolume* volume = new ColouredCubesVolume(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), blockSize, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.
	gColouredCubesVolumes.push_back(volume);
	return gColouredCubesVolumes.size() - 1;
}

CUBIQUITYC_API int32_t cuNewColouredCubesVolumeFromVolDat(const char* volDatToImport, uint32_t blockSize, uint32_t baseNodeSize)
{
	ColouredCubesVolume* volume = importVolDat<ColouredCubesVolume>(volDatToImport, 256, 256);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.
	gColouredCubesVolumes.push_back(volume);
	return gColouredCubesVolumes.size() - 1;
}

CUBIQUITYC_API void cuUpdateVolume(int32_t volumeHandle)
{
	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);

	volume->update(Vector3F(0.0f, 0.0f, 0.0f), 0);
}

CUBIQUITYC_API void cuDeleteColouredCubesVolume(int32_t volumeHandle)
{
	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	delete volume;

	// In the future we could consider reusing this slot as we can detect that it set to zero.
	gColouredCubesVolumes[volumeHandle] = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Octree functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API int32_t cuGetRootOctreeNode(int32_t volumeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getRootOctreeNode();

	return node->mSelf;
}

CUBIQUITYC_API int32_t cuGetChildNode(int32_t volumeHandle, int32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ)
{
	OctreeNode<Colour>* node = getNodeFromHandle(volumeHandle, nodeHandle);
	OctreeNode<Colour>* child = node->getChildNode(childX, childY, childZ);
	if(child)
	{
		return child->mSelf;
	}
	else
	{
		return -1;
	}
}

CUBIQUITYC_API int32_t cuNodeHasMesh(int32_t volumeHandle, int32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(volumeHandle, nodeHandle);
	return node->mPolyVoxMesh != 0;
}

////////////////////////////////////////////////////////////////////////////////
// Mesh functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API uint32_t cuGetNoOfVertices(int32_t volumeHandle, int32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(volumeHandle, nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	return polyVoxMesh->getNoOfVertices();
}

CUBIQUITYC_API uint32_t cuGetNoOfIndices(int32_t volumeHandle, int32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(volumeHandle, nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	return polyVoxMesh->getNoOfIndices();
}

CUBIQUITYC_API float* cuGetVertices(int32_t volumeHandle, int32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(volumeHandle, nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< typename VoxelTraits<Colour>::VertexType >& vertexVector = polyVoxMesh->getVertices();

	const VoxelTraits<Colour>::VertexType* vertexPointer = &(vertexVector[0]);

	const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);

	float* floatPointer = const_cast<float*>(constFloatPointer);

	return floatPointer;
}

CUBIQUITYC_API uint32_t* cuGetIndices(int32_t volumeHandle, int32_t nodeHandle)
{
	OctreeNode<Colour>* node = getNodeFromHandle(volumeHandle, nodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< unsigned int >& indexVector = polyVoxMesh->getIndices();
	const unsigned int* constUIntPointer = &(indexVector[0]);

	unsigned int* uintPointer = const_cast<unsigned int*>(constUIntPointer);

	return uintPointer;
}
