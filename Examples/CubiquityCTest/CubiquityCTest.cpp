#include <iostream>
#include <sstream>

#include "CubiquityC.h"

void validate(int returnCode)
{
	if (returnCode != CU_OK)
	{
		std::cout << cuGetErrorCodeAsString(returnCode) << " : " << cuGetLastErrorMessage() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void testColoredCubesVolume()
{
	int width = 128;
	int height = 32;
	int depth = 128;
	unsigned int volumeID;
	// Empty path signifies temporary database. 32 is the internal chunk size (not imporant).
	validate(cuNewEmptyColoredCubesVolume(0, 0, 0, width - 1, height - 1, depth - 1, "", 32, &volumeID));

	// Precompute the colors to avoid calling 'cuMakeColor' inside the loop.
	CuColor redCol = cuMakeColor(255, 0, 0, 255);
	CuColor greenCol = cuMakeColor(0, 255, 0, 255);
	CuColor blueCol = cuMakeColor(0, 0, 255, 255);

	// Iterate over each voxel
	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				// Write a color based on the height of the voxel.
				if (y < 5)
					validate(cuSetVoxel(volumeID, x, y, z, &redCol));
				else if (y < 10)
					validate(cuSetVoxel(volumeID, x, y, z, &greenCol));
				else if (y < 15)
					validate(cuSetVoxel(volumeID, x, y, z, &blueCol));
			}
		}
	}

	// This basically commits our changes.
	validate(cuAcceptOverrideChunks(volumeID));

	// Retieve the stored voxel to check it worked
	CuColor storedColor;
	validate(cuGetVoxel(volumeID, 10, 10, 10, &storedColor));

	// Extract the color components
	uint8_t red, green, blue, alpha;
	cuGetAllComponents(storedColor, &red, &green, &blue, &alpha);

	// Print out the retrieved color
	std::stringstream ss;
	ss << "Stored voxel color is (" << (int)red << ", " << (int)green << ", " << (int)blue << ", " << (int)alpha << ")" << std::endl;
	std::cout << ss.str();

	// Delete the volume from meory (doesn't delete from disk).
	validate(cuDeleteVolume(volumeID));
}

void processOctreeNode(uint32_t octreeNodeHandle)
{
	CuOctreeNode octreeNode;
	cuGetOctreeNode(octreeNodeHandle, &octreeNode);

	std::cout << "Node position: " << octreeNode.posX << " " << octreeNode.posY << " " << octreeNode.posZ << std::endl;

	if (octreeNode.hasMesh)
	{
		// These will point to the index and vertex data
		uint32_t noOfIndices;
		uint16_t* indices;
		uint16_t noOfVertices;
		void* vertices;

		// Get the index and vertex data
		validate(cuGetMesh(octreeNodeHandle, &noOfVertices, &vertices, &noOfIndices, &indices));

		std::cout << "Found mesh - it has " << noOfVertices << " vertices and " << noOfIndices << " indices." << std::endl;
	}

	for (uint32_t z = 0; z < 2; z++)
	{
		for (uint32_t y = 0; y < 2; y++)
		{
			for (uint32_t x = 0; x < 2; x++)
			{
				if (octreeNode.childHandles[x][y][z] != 0xFFFFFFFF)
				{
					// Recursivly call the octree traversal
					processOctreeNode(octreeNode.childHandles[x][y][z]);
				}
			}
		}
	}
}

void testTerrainVolume()
{
	uint32_t volumeHandle;
	validate(cuNewTerrainVolumeFromVDB("C:/code/cubiquity/Data/Volumes/Version 0/SmoothVoxeliensTerrain.vdb", CU_READONLY, 32, &volumeHandle));

	validate(cuUpdateVolume(volumeHandle, 0.0f, 0.0f, 0.0f, 0.0f));

	uint32_t hasRootNode;
	validate(cuHasRootOctreeNode(volumeHandle, &hasRootNode));
	if(hasRootNode == 1)
	{
		uint32_t octreeNodeHandle;
		cuGetRootOctreeNode(volumeHandle, &octreeNodeHandle);
		processOctreeNode(octreeNodeHandle);
	}

	// Delete the volume from memory (doesn't delete from disk).
	validate(cuDeleteVolume(volumeHandle));
}

int main()
{
	testColoredCubesVolume();
	testTerrainVolume();
	
	return 0;
}