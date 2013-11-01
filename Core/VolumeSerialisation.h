#ifndef CUBIQUITY_VOLUMESERIALISATION_H_
#define CUBIQUITY_VOLUMESERIALISATION_H_

#include "ColoredCubesVolume.h"
#include "CubiquityForwardDeclarations.h"
#include "TerrainVolume.h"
#include "UpdatePriorities.h"

#include "PolyVoxCore\Impl\ErrorHandling.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "boost/filesystem.hpp"

#include <climits>
#include <map>
#include <string>

namespace Cubiquity
{
	std::map<std::string, std::string> parseIndexFile(const std::string& filename);
	void convertStringToInt(const std::string& str, int& i);

	ColoredCubesVolumeImpl* importColoredSlices(std::string folder);
	void exportSlices(ColoredCubesVolumeImpl* volume, std::string folder);

	TerrainVolumeImpl* importSmoothSlices(std::string folder);
	void exportSlices(TerrainVolumeImpl* volume, std::string folder);

	template <typename VoxelType>
	void pixelToVoxel(uint8_t* pixelData, VoxelType& voxelData, uint32_t componentCount)
	{
		POLYVOX_ASSERT(false, "NOT IMPLEMENTED");
	}

	void pixelToVoxel(uint8_t* pixelData, Color& voxelData, uint32_t componentCount);
	void pixelToVoxel(uint8_t* pixelData, MaterialSet& voxelData, uint32_t componentCount);

	template <typename VoxelType>
	void voxelToPixel(VoxelType& voxelData, uint8_t* pixelData, uint32_t componentCount)
	{
		POLYVOX_ASSERT(false, "NOT IMPLEMENTED");
	}

	void voxelToPixel(Color& voxelData, uint8_t* pixelData, uint32_t componentCount);
	void voxelToPixel(MaterialSet& voxelData, uint8_t* pixelData, uint32_t componentCount);

	ColoredCubesVolumeImpl* importVxl(const std::string& vxlFfilename, const std::string& volumeFilename);

	ColoredCubesVolumeImpl* importHeightmap(const std::string& heightmapFileName, const std::string& colormapFileName, const std::string& volumeFilename, uint32_t baseNodeSize);

	// --------------------------------------------------
	// Imports data in the VolDat format.
	// --------------------------------------------------
	template <typename CubiquityVolumeType>
	CubiquityVolumeType* importVolDat(std::string folder, const std::string& volumeFilename, uint32_t baseNodeSize)
	{
		logInfo() << "Importing images from '" << folder << "' and into '" << volumeFilename << "'";
		if((folder.back() != '/') && (folder.back() != '\\'))
		{
			//logWarning() << "Folder name " << folder << " is missing a trailing '/' or '\\'. Please to provide this to avoid confusion!";
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

					// Using 'C' file IO due to Gameplay3D issues.
					FILE* fp;
					fp = fopen(filename.c_str(), "rb");
					if(fp)
					{
						logDebug() << "Found image '" << filename << "' for import";
						imageFilenames.push_back(filename);
						foundImage = true;
						break;
					}
				}
			}

			image++;
		} while(foundImage);

		// Make sure at least one image was found.
		uint32_t sliceCount = imageFilenames.size();
		POLYVOX_THROW_IF(sliceCount == 0, std::invalid_argument, "No images found in provided folder");

		// Open the first image to determine the width and height
		int volumeWidth = 0, volumeHeight = 0, noOfChannels;
		unsigned char *sliceData = stbi_load((*(imageFilenames.begin())).c_str(), &volumeWidth, &volumeHeight, &noOfChannels, 0);

		// Make sure it opened sucessfully
		POLYVOX_THROW_IF(sliceData == NULL, std::runtime_error, "Failed to open first image");

		//Close it straight away - we only wanted to find the dimensions.
		stbi_image_free(sliceData);

		//Create the volume
		// When importing we treat 'y' as up because the Gameplay physics engine makes some
		// assumptions about this. This means we need to swap the 'y' and 'slice' indices.
		CubiquityVolumeType* volume = new CubiquityVolumeType(Region(0, 0, 0, volumeWidth - 1, sliceCount - 1, volumeHeight - 1), volumeFilename, baseNodeSize);

		// Now iterate over each slice and import the data.
		for(int slice = 0; slice < sliceCount; slice++)
		{
			int imageWidth = 0, imageHeight = 0, imageChannels;
			unsigned char *sliceData = stbi_load(imageFilenames[slice].c_str(), &imageWidth, &imageHeight, &imageChannels, 0);

			POLYVOX_THROW_IF(imageWidth != volumeWidth, std::runtime_error, "All images are not the same width!");
			POLYVOX_THROW_IF(imageHeight != volumeHeight, std::runtime_error, "All images are not the same height!");

			// Now iterate over each pixel.
			for(int x = 0; x < imageWidth; x++)
			{
				for(int y = 0; y < imageHeight; y++)
				{
					unsigned char *pixel = sliceData + (y * imageWidth + x) * imageChannels;

					CubiquityVolumeType::VoxelType voxel;
					pixelToVoxel(pixel, voxel, imageChannels);

					// When importing we treat 'y' as up because the Gameplay physics engine makes some
					// assumptions about this. This means we need to swap the 'y' and 'slice' indices.
					volume->setVoxelAt(x, slice, y, voxel, UpdatePriorities::DontUpdate);
				}
			}

			stbi_image_free(sliceData);
		}

		volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Background);

		return volume;
	}

	// --------------------------------------------------
	// Exports data in the VolDat format.
	// --------------------------------------------------
	template <typename CubiquityVolumeType>
	void exportVolDat(CubiquityVolumeType* volume, std::string folder)
	{
		boost::filesystem::path path(folder);
		if((!is_directory(path)) && (!boost::filesystem::create_directory(path)))
		{
			POLYVOX_THROW(std::runtime_error, "Failed to create directory \'" + folder + "\'for export");
		}

		// Note that 'y' and 'z' axis are flipped as Gameplay physics engine assumes 'y' is up.
		uint32_t imageWidth = volume->getWidth();
		uint32_t imageHeight = volume->getDepth();
		uint32_t sliceCount = volume->getHeight();
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
				for(uint32_t y = 0; y < imageHeight; y++)
				{
					unsigned char* pixel = outputSliceData + (y * imageWidth + x) * componentCount;

					// Note that 'y' and 'z' axis are flipped as Gameplay physics engine assumes 'y' is up.
					CubiquityVolumeType::VoxelType voxel = volume->getVoxelAt(x, slice, y);

					voxelToPixel(voxel, pixel, componentCount);
				}
			}

			// Now save the slice data as an image file.
			std::stringstream ss;
			ss << folder << std::setfill('0') << std::setw(6) << slice << "." << sliceExtension;
			int result = stbi_write_png(ss.str().c_str(), imageWidth, imageHeight, componentCount, outputSliceData, imageWidth * componentCount);
			assert(result); //If crashing here then make sure the output folder exists.
		}

		delete[] outputSliceData;

		FILE *fp;
		fp=fopen((folder + "Volume.idx").c_str(), "w");
		fprintf(fp, "Width = %d\n", imageWidth);
		fprintf(fp, "Height = %d\n", imageHeight);
		fprintf(fp, "SliceCount = %d\n", sliceCount);
		fprintf(fp, "SliceExtension = %s\n", sliceExtension.c_str());
		fprintf(fp, "ComponentCount = %d\n", componentCount);
		fprintf(fp, "ComponentType = %s\n", componentType.c_str());
		fprintf(fp, "ComponentSize = %d\n", componentSize);
		
		fclose(fp);
	}
}

#endif //CUBIQUITY_VOLUMESERIALISATION_H_
