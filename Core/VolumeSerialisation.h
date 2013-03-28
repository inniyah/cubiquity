#ifndef CUBIQUITY_VOLUMESERIALISATION_H_
#define CUBIQUITY_VOLUMESERIALISATION_H_

#include "ColouredCubesVolume.h"
#include "CubiquityForwardDeclarations.h"
#include "SmoothTerrainVolume.h"
#include "UpdatePriorities.h"

#include "PolyVoxCore\Impl\ErrorHandling.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "boost/filesystem.hpp"

#include <climits>
#include <map>

namespace Cubiquity
{
	std::map<std::string, std::string> parseIndexFile(const std::string& filename);
	void convertStringToInt(const std::string& str, int& i);

	ColouredCubesVolume* importColouredSlices(std::string folder);
	void exportSlices(ColouredCubesVolume* volume, std::string folder);

	SmoothTerrainVolume* importSmoothSlices(std::string folder);
	void exportSlices(SmoothTerrainVolume* volume, std::string folder);

	template <typename VoxelType>
	void pixelToVoxel(uint8_t* pixelData, VoxelType& voxelData, uint32_t componentCount)
	{
		POLYVOX_ASSERT(false, "NOT IMPLEMENTED");
	}

	void pixelToVoxel(uint8_t* pixelData, Colour& voxelData, uint32_t componentCount);
	void pixelToVoxel(uint8_t* pixelData, MultiMaterial& voxelData, uint32_t componentCount);

	template <typename VoxelType>
	void voxelToPixel(VoxelType& voxelData, uint8_t* pixelData, uint32_t componentCount)
	{
		POLYVOX_ASSERT(false, "NOT IMPLEMENTED");
	}

	void voxelToPixel(Colour& voxelData, uint8_t* pixelData, uint32_t componentCount);
	void voxelToPixel(MultiMaterial& voxelData, uint8_t* pixelData, uint32_t componentCount);

	ColouredCubesVolume* importVxl(const std::string& filename);

	// --------------------------------------------------
	// Imports data in the VolDat format.
	// --------------------------------------------------
	template <typename VolumeType>
	VolumeType* importVolDat(std::string folder)
	{
		std::string indexFileName(folder);
		indexFileName = indexFileName + "Volume.idx";
		std::map<std::string, std::string> index = parseIndexFile(indexFileName);

		//Create the volume
		int volumeWidth;    convertStringToInt(index["Width"], volumeWidth);
		int volumeHeight;   convertStringToInt(index["Height"], volumeHeight);
		int sliceCount;     convertStringToInt(index["SliceCount"], sliceCount);
		int componentCount; convertStringToInt(index["ComponentCount"], componentCount);

		// When importing we treat 'y' as up because the Gameplay physics engine makes some
		// assumptions about this. This means we need to swap the 'y' and 'slice' indices.
		VolumeType* volume = new VolumeType(0, 0, 0, volumeWidth - 1, sliceCount - 1, volumeHeight - 1, 32, 32);

		// Now iterate over each slice and import the data.
		for(int slice = 0; slice < sliceCount; slice++)
		{
			std::stringstream ss;
			ss << folder << std::setfill('0') << std::setw(6) << slice << "." << index["SliceExtension"];
			std::string imageFileName = ss.str();

			int imageWidth = 0, imageHeight = 0, imageChannels;
			unsigned char *sliceData = stbi_load(imageFileName.c_str(), &imageWidth, &imageHeight, &imageChannels, 0);
			assert(imageWidth == volumeWidth);
			assert(imageHeight == volumeHeight);
			assert(imageChannels == componentCount);

			// Now iterate over each pixel.
			for(int x = 0; x < imageWidth; x++)
			{
				for(int y = 0; y < imageHeight; y++)
				{
					unsigned char *pixel = sliceData + (y * imageWidth + x) * imageChannels;

					VolumeType::VoxelType voxel;
					pixelToVoxel(pixel, voxel, componentCount);

					// When importing we treat 'y' as up because the Gameplay physics engine makes some
					// assumptions about this. This means we need to swap the 'y' and 'slice' indices.
					volume->setVoxelAt(x, slice, y, voxel, UpdatePriorities::DontUpdate);
				}
			}
		}

		volume->markAsModified(volume->mPolyVoxVolume->getEnclosingRegion(), UpdatePriorities::Background);

		return volume;
	}

	// --------------------------------------------------
	// Exports data in the VolDat format.
	// --------------------------------------------------
	template <typename VolumeType>
	void exportVolDat(VolumeType* volume, std::string folder)
	{
		boost::filesystem::path path(folder);
		if((!is_directory(path)) && (!boost::filesystem::create_directory(path)))
		{
			POLYVOX_THROW(std::runtime_error, "Failed to create directory \'" + folder + "\'for export");
		}

		// Note that 'y' and 'z' axis are flipped as Gameplay physics engine assumes 'y' is up.
		uint32_t imageWidth = volume->mPolyVoxVolume->getWidth();
		uint32_t imageHeight = volume->mPolyVoxVolume->getDepth();
		uint32_t sliceCount = volume->mPolyVoxVolume->getHeight();
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
					VolumeType::VoxelType voxel = volume->mPolyVoxVolume->getVoxel(x, slice, y);

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

	template <typename VolumeType>
	void loadData(const char* filename, VolumeType* volume)
	{
		FILE* inputFile = fopen(filename, "rb");
		if(!inputFile)
		{
			GP_ERROR("Failed to open volume file");
		}

		fseek(inputFile, 6, SEEK_SET);

		typename VolumeType::VoxelType value;

		//This three-level for loop iterates over every voxel in the volume
		for (int z = 0; z < volume->mPolyVoxVolume->getWidth(); z++)
		{
			for (int y = 0; y < volume->mPolyVoxVolume->getHeight(); y++)
			{
				for (int x = 0; x < volume->mPolyVoxVolume->getDepth(); x++)
				{
					//Slow and inefficient reading one voxel at a time!
					size_t elementsRead = fread(&value, sizeof(typename VolumeType::VoxelType), 1, inputFile);

					if(elementsRead != 1)
					{
						GP_ERROR("Failed to read voxel %d, %d, %d", x, y, z);
					}

					//Write the voxel value into the volume
					volume->setVoxelAt(x, y, z, value, UpdatePriorities::DontUpdate); //Don't mark as dirty - we'll do that at the end.

					/*if(y < 8)
					{
						MultiMaterial mat;
						mat.setMaterial(0, 255);
						volume->setVoxelAt(x, y, z, mat);
					}*/

				}
			}
		}

		fclose(inputFile);

		volume->markAsModified(volume->mPolyVoxVolume->getEnclosingRegion());
	}

	template <typename VolumeType>
	void saveData(const char* filename, VolumeType* volume)
	{
		FILE* outputFile = fopen(filename, "wb");
		if(!outputFile)
		{
			POLYVOX_ASSERT(false, "Failed to open volume file");
		}

		uint16_t width = volume->mPolyVoxVolume->getWidth();
		uint16_t height = volume->mPolyVoxVolume->getHeight();
		uint16_t depth = volume->mPolyVoxVolume->getDepth();
		fwrite(&width, sizeof(uint16_t), 1, outputFile);
		fwrite(&height, sizeof(uint16_t), 1, outputFile);
		fwrite(&depth, sizeof(uint16_t), 1, outputFile);

		//This three-level for loop iterates over every voxel in the volume
		for (int z = 0; z < volume->mPolyVoxVolume->getWidth(); z++)
		{
			for (int y = 0; y < volume->mPolyVoxVolume->getHeight(); y++)
			{
				for (int x = 0; x < volume->mPolyVoxVolume->getDepth(); x++)
				{
					typename VolumeType::VoxelType value = volume->getVoxelAt(x,y,z);
					fwrite(&value, sizeof(typename VolumeType::VoxelType), 1, outputFile);
				}
			}
		}

		fclose(outputFile);
	}
}

#endif //CUBIQUITY_VOLUMESERIALISATION_H_
