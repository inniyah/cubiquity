#include "ImportImageSlices.h"

#include "HeaderOnlyLibs.h"

#include "CubiquityC.h"

#include <iomanip>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <vector>

using namespace std;

std::vector<std::string> findImagesInFolder(std::string folder)
{
	if((folder.back() != '/') && (folder.back() != '\\'))
	{
		folder.append("/");
	}

	// Slightly hacky way to initialise vector: http://stackoverflow.com/a/8906577
	std::string extsArray[] = { "png", "PNG", "jpg", "JPG", "jpeg", "JPEG", "bmp", "BMP" };
	std::vector<std::string> extensions(std::begin(extsArray), std::end(extsArray));

	// Identify all relevant images
	uint32_t image = 0;
	bool foundImage = false;
	std::vector<std::string> imageFilenames;
	do
	{
		foundImage = false;

		// Note: The number of leading zeros may not be the same for all files. e.g. 009.png and 010.png.
		// The total number of chaachters may also be different - e.g. 1.png and 324.png.
		for(uint32_t leadingZeros = 0; (leadingZeros < 10) && (foundImage == false) ; leadingZeros++)
		{
			std::stringstream filenameBase;
			filenameBase << std::setfill('0') << std::setw(leadingZeros) << image;

			//for(std::string ext : extensions)
			for(auto extIter = extensions.begin(); extIter != extensions.end(); extIter++)
			{
				std::string filename = folder + filenameBase.str() + "." + *(extIter);

				// Check whether the file exists
				FILE* fp;
				fp = fopen(filename.c_str(), "rb");
				if(fp)
				{
					imageFilenames.push_back(filename);
					foundImage = true;
					fclose(fp);
					break;
				}
			}
		}

		image++;
	} while(foundImage);

	return imageFilenames;
}

bool importImageSlices(ez::ezOptionParser& options)
{
	string folder;
	options.get("-imageslices")->getString(folder);
	string pathToVoxelDatabase;
	options.get("-coloredcubes")->getString(pathToVoxelDatabase);

	cout << "Importing images from '" << folder << "' and into '" << pathToVoxelDatabase << "'";

	std::vector<std::string> imageFilenames = findImagesInFolder(folder);

	// Make sure at least one image was found.
	uint32_t sliceCount = imageFilenames.size();
	if(sliceCount == 0)
	{
		cerr << "No images found in provided folder" << endl;
		return false;
	}
	cout << "Found " << imageFilenames.size() << " images for import" << endl;

	// Open the first image to determine the width and height
	int volumeWidth = 0, volumeHeight = 0, noOfChannels;
	unsigned char *sliceData = stbi_load((*(imageFilenames.begin())).c_str(), &volumeWidth, &volumeHeight, &noOfChannels, 0);

	// Make sure it opened sucessfully
	if(sliceData == NULL)
	{
		cerr << "Failed to open first image" << endl;
		return false;
	}

	//Close it straight away - we only wanted to find the dimensions.
	stbi_image_free(sliceData);

	// Create the volume. When importing we treat 'y' as up because most game engines and
	// physics engines expect this. This means we need to swap the 'y' and 'slice' indices.
	uint32_t volumeHandle;
	if(cuNewEmptyColoredCubesVolume(0, 0, 0, volumeWidth - 1, sliceCount - 1, volumeHeight - 1, pathToVoxelDatabase.c_str(), 32, &volumeHandle) != CU_OK)
	{
		cerr << "Failed to create new empty volume" << endl;
		return false;
	}

	// Now iterate over each slice and import the data.
	for(int slice = 0; slice < sliceCount; slice++)
	{
		cout << "Importing image " << slice << endl;
		int imageWidth = 0, imageHeight = 0, imageChannels;
		unsigned char *sliceData = stbi_load(imageFilenames[slice].c_str(), &imageWidth, &imageHeight, &imageChannels, 0);

		if((imageWidth != volumeWidth) || (imageHeight != volumeHeight))
		{
			cerr << "All images must have the same dimensions!" << endl;
			return false;
		}

		if(imageChannels != 4)
		{
			// When building a colored cubes volume we need an alpha channel in the images
			// or we do not know what to write into the alpha channel of the volume.
			cerr << "Image does not have an alpha channel but one is required" << endl;
			return false;
		}

		// Now iterate over each pixel.
		for(int x = 0; x < imageWidth; x++)
		{
			for(int y = 0; y < imageHeight; y++)
			{
				unsigned char *pixelData = sliceData + (y * imageWidth + x) * imageChannels;

				CuColor color = cuMakeColor(*(pixelData + 0), *(pixelData + 1), *(pixelData + 2), *(pixelData + 3));

				// When importing we treat 'y' as up because most game engines and physics
				// engines expect this. This means we need to swap the 'y' and 'slice' indices.
				if(cuSetVoxel(volumeHandle, x, slice, y, &color) != CU_OK)
				{
					cerr << "Error setting voxel color" << endl;
					return false;
				}
			}
		}

		stbi_image_free(sliceData);
	}

	//volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Background);

	cuAcceptOverrideChunks(volumeHandle);
	cuDeleteVolume(volumeHandle);

	return true;
}
