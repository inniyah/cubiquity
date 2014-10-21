#include "ImportHeightmap.h"

#include "CubiquityC.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.cpp"

#include <iomanip>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <vector>

using namespace std;

bool importHeightmap(const std::string& heightmapFilename, const std::string& colormapFilename, const std::string& pathToVoxelDatabase, uint32_t outputFormat)
{
	cout << "Importing '" << heightmapFilename << "' as heightmap and '" << colormapFilename << "' as colormap" << endl;
	cout << "Writing result to '" << pathToVoxelDatabase << "'" << endl;

	if (outputFormat == CU_COLORED_CUBES)
	{
		return importHeightmapAsColoredCubesVolume(heightmapFilename, colormapFilename, pathToVoxelDatabase);
	}
	else
	{
		return false;
	}
}

bool importHeightmapAsColoredCubesVolume(const std::string& heightmapFilename, const std::string& colormapFilename, const std::string& pathToVoxelDatabase)
{
	// Open the heightmap
	int heightmapWidth = 0, heightmapHeight = 0, heightmapChannels;
	unsigned char* heightmapData = stbi_load(heightmapFilename.c_str(), &heightmapWidth, &heightmapHeight, &heightmapChannels, 0);

	// Make sure it opened sucessfully
	if (heightmapData == NULL)
	{
		cerr << "Failed to open heightmap" << endl;
		return false;
	}

	// Open the colormap
	int colormapWidth = 0, colormapHeight = 0, colormapChannels;
	unsigned char* colormapData = stbi_load(colormapFilename.c_str(), &colormapWidth, &colormapHeight, &colormapChannels, 0);

	// Make sure it opened sucessfully
	if (colormapData == NULL)
	{
		cerr << "Failed to open colormap" << endl;
		return false;
	}

	if ((heightmapWidth != colormapWidth) || (heightmapHeight != colormapHeight))
	{
		cerr << "Heightmap and colormap must have same dimensions" << endl;
	}

	// Create the volume. When importing we treat 'y' as up because most game engines and
	// physics engines expect this. This means we need to swap the 'y' and 'slice' indices.
	uint32_t volumeHandle;
	uint32_t volumeWidth = heightmapWidth;
	uint32_t volumeHeight = 256; // Assume we're not loading HDR images, not supported by stb_image anyway
	uint32_t volumeDepth = heightmapHeight;
	if (cuNewEmptyColoredCubesVolume(0, 0, 0, volumeWidth - 1, volumeHeight - 1, volumeDepth - 1, pathToVoxelDatabase.c_str(), 32, &volumeHandle) != CU_OK)
	{
		cerr << "Failed to create new empty volume" << endl;
		return false;
	}

	CuColor gray = cuMakeColor(63, 63, 63, 255);
	CuColor empty = cuMakeColor(0, 0, 0, 0);

	for (uint32_t imageY = 0; imageY < heightmapHeight; imageY++)
	{
		for (uint32_t imageX = 0; imageX < heightmapWidth; imageX++)
		{
			unsigned char* heightmapPixel = heightmapData + (imageY * heightmapWidth + imageX) * heightmapChannels;
			unsigned char* colormapPixel = colormapData + (imageY * colormapWidth + imageX) * colormapChannels;

			for (uint32_t height = 0; height < volumeHeight; height++)
			{
				CuColor colorToUse;
				if (height < *heightmapPixel)
				{
					colorToUse = gray;
				}
				else if (height == *heightmapPixel)
				{
					colorToUse = cuMakeColor(*(colormapPixel + 0), *(colormapPixel + 1), *(colormapPixel + 2), 255);
				}
				else
				{
					colorToUse = empty;
				}

				if (cuSetVoxel(volumeHandle, imageX, height, imageY, &colorToUse) != CU_OK)
				{
					cerr << "Error setting voxel color" << endl;
					return false;
				}
			}
		}
	}

	cuAcceptOverrideChunks(volumeHandle);
	cuDeleteVolume(volumeHandle);

	return true;
}