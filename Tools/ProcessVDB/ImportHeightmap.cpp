#include "ImportHeightmap.h"

#include "CubiquityC.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.cpp"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_DEFAULT_FILTER_UPSAMPLE     STBIR_FILTER_CATMULLROM
#include "stb_image_resize.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <vector>

using namespace std;

bool importHeightmap(ez::ezOptionParser& options)
{
	//cout << "Importing '" << heightmapFilename << "' as heightmap and '" << colormapFilename << "' as colormap" << endl;
	//cout << "Writing result to '" << pathToVoxelDatabase << "'" << endl;

	//if (outputFormat == CU_COLORED_CUBES)
	if (options.isSet("-coloredcubes"))
	{
		return importHeightmapAsColoredCubesVolume(options);
	}
	//else if (outputFormat == CU_TERRAIN)
	else if (options.isSet("-terrain"))
	{
		return importHeightmapAsTerrainVolume(options);
	}
	else
	{
		return false;
	}
}

bool importHeightmapAsColoredCubesVolume(ez::ezOptionParser& options)
{
	// Open the heightmap
	int heightmapWidth = 0, heightmapHeight = 0, heightmapChannels;
	string heightmapFilename;
	options.get("-heightmap")->getString(heightmapFilename);
	unsigned char* heightmapData = stbi_load(heightmapFilename.c_str(), &heightmapWidth, &heightmapHeight, &heightmapChannels, 0);

	// Make sure it opened sucessfully
	if (heightmapData == NULL)
	{
		cerr << "Failed to open heightmap" << endl;
		return false;
	}

	// Open the colormap
	int colormapWidth = 0, colormapHeight = 0, colormapChannels;
	string colormapFilename;
	options.get("-colormap")->getString(colormapFilename);
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
	string pathToVoxelDatabase;
	options.get("-coloredcubes")->getString(pathToVoxelDatabase);
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

bool importHeightmapAsTerrainVolume(ez::ezOptionParser& options)
{
	// Open the heightmap
	int originalWidth = 0, originalHeight = 0, originalChannels; // 'Original' values are before resampling
	string heightmapFilename;
	options.get("-heightmap")->getString(heightmapFilename);
	unsigned char* originalData = stbi_load(heightmapFilename.c_str(), &originalWidth, &originalHeight, &originalChannels, 0);

	// Make sure it opened sucessfully
	if (originalData == NULL)
	{
		cerr << "Failed to open heightmap" << endl;
		return false;
	}

	int originalPixelCount = originalWidth * originalHeight;
	float* floatOriginalData = new float[originalPixelCount];
	for (int y = 0; y < originalHeight; y++)
	{
		for (int x = 0; x < originalWidth; x++)
		{
			floatOriginalData[y * originalWidth + x] = static_cast<float>(originalData[(y * originalWidth + x) * originalChannels]) / 255.0f;
		}
	}

	int resampledWidth = 512;
	int resampledHeight = 512;
	int resampledPixelCount = resampledWidth * resampledHeight;
	float* resampledData = new float[resampledPixelCount];
	/*for (int ct = 0; ct < resampledPixelCount; ct++)
	{
		resampledData[ct] = floatOriginalData[ct];
	}*/

	stbir_resize_float(floatOriginalData, originalWidth, originalHeight, 0,
		resampledData, resampledWidth, resampledHeight, 0, 1);

	// Create the volume. When importing we treat 'y' as up because most game engines and
	// physics engines expect this. This means we need to swap the 'y' and 'slice' indices.
	uint32_t volumeHandle;
	uint32_t volumeWidth = resampledWidth;
	uint32_t volumeHeight = 64; // Assume we're not loading HDR images, not supported by stb_image anyway
	uint32_t volumeDepth = resampledHeight;
	string pathToVoxelDatabase;
	options.get("-terrain")->getString(pathToVoxelDatabase);
	if (cuNewEmptyTerrainVolume(0, 0, 0, volumeWidth - 1, volumeHeight - 1, volumeDepth - 1, pathToVoxelDatabase.c_str(), 32, &volumeHandle) != CU_OK)
	{
		cerr << "Failed to create new empty volume" << endl;
		return false;
	}

	for (uint32_t imageY = 0; imageY < resampledHeight; imageY++)
	{
		for (uint32_t imageX = 0; imageX < resampledWidth; imageX++)
		{
			float fPixelHeight = resampledData[imageY * resampledWidth + imageX];

			for (uint32_t height = 0; height < volumeHeight; height++)
			{
				CuMaterialSet materialSet;
				materialSet.data = 0;

				float fHeight = float(height) / float(volumeHeight);				

				float fDiff = fPixelHeight - fHeight;

				fDiff *= 1000.0f;

				int diff = int(fDiff);
				diff += 127;

				/*int diff = (int)height - int(*heightmapPixel);
				diff *= 10;
				diff += 127;*/

				diff = (std::min)(diff, 255);
				diff = (std::max)(diff, 0);

				materialSet.data = static_cast<uint8_t>(diff);

				/*if (height < imageX)
				{
					materialSet.data = 255;
				}
				else
				{
					materialSet.data = 0;
				}*/

				if (cuSetVoxel(volumeHandle, imageX, height, imageY, &materialSet) != CU_OK)
				{
					cerr << "Error setting voxel materials" << endl;
					return false;
				}
			}
		}
	}

	cuAcceptOverrideChunks(volumeHandle);
	cuDeleteVolume(volumeHandle);

	return true;
}
