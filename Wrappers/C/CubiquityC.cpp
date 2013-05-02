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

ColouredCubesVolume* getVolumeFromHandle(uint16_t handle)
{
	return gColouredCubesVolumes[handle];
}

////////////////////////////////////////////////////////////////////////////////
// Volume functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API uint16_t newColouredCubesVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, uint32_t blockSize, uint32_t baseNodeSize)
{
	ColouredCubesVolume* volume = new ColouredCubesVolume(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), blockSize, baseNodeSize);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.
	gColouredCubesVolumes.push_back(volume);
	return gColouredCubesVolumes.size() - 1;
}

CUBIQUITYC_API uint16_t newColouredCubesVolumeFromVolDat(const char* volDatToImport, uint32_t blockSize, uint32_t baseNodeSize)
{
	ColouredCubesVolume* volume = importVolDat<ColouredCubesVolume>(volDatToImport, 256, 256);
	volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Immediate); //Immediate update just while we do unity experiments.
	gColouredCubesVolumes.push_back(volume);
	return gColouredCubesVolumes.size() - 1;
}

CUBIQUITYC_API void updateVolume(uint16_t volumeHandle)
{
	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);

	volume->update(Vector3F(0.0f, 0.0f, 0.0f), 0);
}

CUBIQUITYC_API void deleteColouredCubesVolume(uint16_t volumeHandle)
{
	ColouredCubesVolume* volume = getVolumeFromHandle(volumeHandle);
	delete volume;

	// In the future we could consider reusing this slot as we can detect that it set to zero.
	gColouredCubesVolumes[volumeHandle] = 0;
}

////////////////////////////////////////////////////////////////////////////////
// Octree functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API uint16_t getRootOctreeNode(uint16_t volumeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getRootOctreeNode();

	return node->mSelf;
}

////////////////////////////////////////////////////////////////////////////////
// Mesh functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API unsigned int getNoOfVertices(uint16_t volumeHandle, uint16_t octreeNodeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(octreeNodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	return polyVoxMesh->getNoOfVertices();
}

CUBIQUITYC_API unsigned int getNoOfIndices(uint16_t volumeHandle, uint16_t octreeNodeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(octreeNodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	return polyVoxMesh->getNoOfIndices();
}

CUBIQUITYC_API float* getVertices(uint16_t volumeHandle, uint16_t octreeNodeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(octreeNodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< typename VoxelTraits<Colour>::VertexType >& vertexVector = polyVoxMesh->getVertices();

	const VoxelTraits<Colour>::VertexType* vertexPointer = &(vertexVector[0]);

	const float* constFloatPointer = reinterpret_cast<const float*>(vertexPointer);

	float* floatPointer = const_cast<float*>(constFloatPointer);

	return floatPointer;
}

CUBIQUITYC_API unsigned int* getIndices(uint16_t volumeHandle, uint16_t octreeNodeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(octreeNodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< unsigned int >& indexVector = polyVoxMesh->getIndices();
	const unsigned int* constUIntPointer = &(indexVector[0]);

	unsigned int* uintPointer = const_cast<unsigned int*>(constUIntPointer);

	return uintPointer;
}
