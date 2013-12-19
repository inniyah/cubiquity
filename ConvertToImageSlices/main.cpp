#include "CubiquityC.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

void exportAsImageSlices(const std::string& pathToVDB)
{
	uint32_t volumeHandle = 0;
	if(cuNewColoredCubesVolumeFromVDB(pathToVDB.c_str(), 32, &volumeHandle) != 1) //FIXME - hardcoded value
	{
		cerr << "Error opening VDB database" << endl;
		return;
	}

	int lowerX, lowerY, lowerZ, upperX, upperY, upperZ;
	if(cuGetEnclosingRegion(volumeHandle, &lowerX, &lowerY, &lowerZ, &upperX, &upperY, &upperZ) != 1) //FIXME - hardcoded value
	{
		cerr << "Error geting enclosing region" << endl;
	}

	// Note that 'y' and 'z' axis are flipped as Gameplay physics engine assumes 'y' is up.
	uint32_t imageWidth = upperX - lowerX + 1;
	uint32_t imageHeight = upperZ - lowerZ + 1;
	uint32_t sliceCount = upperY - lowerY + 1;
	std::string sliceExtension("png");
	uint32_t componentCount = 4;
	std::string componentType("u");
	uint32_t componentSize = 8;

	int outputSliceDataSize = imageWidth * imageHeight * componentCount * (componentSize / CHAR_BIT);
	unsigned char* outputSliceData = new unsigned char[outputSliceDataSize];

	for(uint32_t slice = 0; slice < sliceCount; slice++)
	{
		std::fill(outputSliceData, outputSliceData + imageWidth * imageHeight, 0);

		for(uint32_t x = 0; x < imageWidth; x++)
		{
			for(uint32_t z = 0; z < imageHeight; z++)
			{
				unsigned char* pixelData = outputSliceData + (z * imageWidth + x) * componentCount;

				CuColor color;
				if(cuGetVoxel(volumeHandle, x + lowerX, slice + lowerY, z + lowerZ, &color) != 1) //FIXME - hardcoded value

				*(pixelData + 0) = (color.data >> 24) & 0xFF;
				*(pixelData + 1) = (color.data >> 16) & 0xFF;
				*(pixelData + 2) = (color.data >>  8) & 0xFF;
				*(pixelData + 3) = (color.data      ) & 0xFF;

				/*// Note that 'y' and 'z' axis are flipped as Gameplay physics engine assumes 'y' is up.
				CubiquityVolumeType::VoxelType voxel = volume->getVoxelAt(x, slice, y);*/

				//voxelToPixel(voxel, pixel, componentCount);
			}
		}

		// Now save the slice data as an image file.
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(6) << slice << "." << sliceExtension;
		int result = stbi_write_png(ss.str().c_str(), imageWidth, imageHeight, componentCount, outputSliceData, imageWidth * componentCount);
		if(result == 0)
		{
			cerr << "Failed to write " << ss.str() << endl;
		}
	}

	delete[] outputSliceData;
}

int main(int argc, char* argv[])
{

	return 0;
}
