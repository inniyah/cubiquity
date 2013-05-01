// CubiquityC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CubiquityC.h"

#include "OctreeNode.h"
#include "ColouredCubesVolume.h"
#include "VolumeSerialisation.h"

#include <vector>

std::vector<ColouredCubesVolume*> gColouredCubesVolumes;

// This is an example of an exported function.
CUBIQUITYC_API float getZero(void)
{
	return 0.0f;
}

CUBIQUITYC_API float getOne(void)
{
	return 1.0f;
}

////////////////////////////////////////////////////////////////////////////////
// Volume functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API unsigned int newColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
{
	//ColouredCubesVolume* volume = new ColouredCubesVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize);
	ColouredCubesVolume* volume = importVolDat<ColouredCubesVolume>("C:/temp/VoxeliensTerrain/", 256, 256);
	gColouredCubesVolumes.push_back(volume);

	/*int centreX = (lowerX + upperX) / 2;
	int centreY = (lowerY + upperY) / 2;
	int centreZ = (lowerZ + upperZ) / 2;

	//MultiMaterial value;
	//value.setMaterial(0, 255);
	Colour value(255, 0, 255, 255);
	volume->setVoxelAt(centreX, centreY, centreZ, value, Cubiquity::UpdatePriorities::Immediate);*/

	return gColouredCubesVolumes.size() - 1;
}

CUBIQUITYC_API void updateVolume(unsigned int volumeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	volume->update(Vector3F(0.0f, 0.0f, 0.0f), 0);
}

////////////////////////////////////////////////////////////////////////////////
// Octree functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API unsigned int getRootOctreeNode(unsigned int volumeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getRootOctreeNode();

	return node->mSelf;
}

////////////////////////////////////////////////////////////////////////////////
// Mesh functions
////////////////////////////////////////////////////////////////////////////////
CUBIQUITYC_API unsigned int getNoOfVertices(unsigned int volumeHandle, unsigned int octreeNodeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(octreeNodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	return polyVoxMesh->getNoOfVertices();
}

CUBIQUITYC_API unsigned int getNoOfIndices(unsigned int volumeHandle, unsigned int octreeNodeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(octreeNodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	return polyVoxMesh->getNoOfIndices();
}

CUBIQUITYC_API float* getVertices(unsigned int volumeHandle, unsigned int octreeNodeHandle)
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

CUBIQUITYC_API unsigned int* getIndices(unsigned int volumeHandle, unsigned int octreeNodeHandle)
{
	ColouredCubesVolume* volume = gColouredCubesVolumes[volumeHandle];

	OctreeNode<Colour>* node = volume->getOctree()->getNodeFromIndex(octreeNodeHandle);

	const ::PolyVox::SurfaceMesh< typename VoxelTraits<Colour>::VertexType >* polyVoxMesh = node->mPolyVoxMesh;

	const std::vector< unsigned int >& indexVector = polyVoxMesh->getIndices();
	const unsigned int* constUIntPointer = &(indexVector[0]);

	unsigned int* uintPointer = const_cast<unsigned int*>(constUIntPointer);

	return uintPointer;
}
