#ifndef CUBIQUITY_VOLUMESERIALISATION_H_
#define CUBIQUITY_VOLUMESERIALISATION_H_

#include "ColouredCubesVolume.h"
#include "CubiquityForwardDeclarations.h"
#include "SmoothTerrainVolume.h"
#include "UpdatePriorities.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
	void pixelToVoxel(uint8_t* pixelData, VoxelType& voxelData, uint8_t componentCount)
	{
		POLYVOX_ASSERT(false, "NOT IMPLEMENTED");
	}

	void pixelToVoxel(uint8_t* pixelData, Colour& voxelData, uint8_t componentCount);
	void pixelToVoxel(uint8_t* pixelData, MultiMaterial& voxelData, uint8_t componentCount);

	template <typename VoxelType>
	void voxelToPixel(VoxelType& voxelData, uint8_t* pixelData, uint8_t componentCount)
	{
		POLYVOX_ASSERT(false, "NOT IMPLEMENTED");
	}

	void voxelToPixel(Colour& voxelData, uint8_t* pixelData, uint8_t componentCount);
	void voxelToPixel(MultiMaterial& voxelData, uint8_t* pixelData, uint8_t componentCount);

	template <typename VolumeType>
	VolumeType* importVolDat(std::string folder)
	{
		string indexFileName(folder);
		indexFileName = indexFileName + "\\Volume.idx";
		std::map<std::string, std::string> index = parseIndexFile(indexFileName);

		//Create the volume
		int volumeWidth;    convertStringToInt(index["Width"], volumeWidth);
		int volumeHeight;   convertStringToInt(index["Height"], volumeHeight);
		int sliceCount;     convertStringToInt(index["SliceCount"], sliceCount);
		int componentCount; convertStringToInt(index["ComponentCount"], componentCount);

		VolumeType* volume = new VolumeType(0, 0, 0, volumeWidth - 1, volumeHeight - 1, sliceCount - 1, 32, 32);
		for(int slice = 0; slice < sliceCount; slice++)
		{
			std::stringstream ss;
			ss << folder << "\\" << setfill('0') << setw(6) << slice << "." << index["SliceExtension"];
			string imageFileName = ss.str();

			int imageWidth = 0, imageHeight = 0, imageChannels;
			unsigned char *sliceData = stbi_load(imageFileName.c_str(), &imageWidth, &imageHeight, &imageChannels, 0);
			assert(imageWidth == volumeWidth);
			assert(imageHeight == volumeHeight);
			assert(imageChannels == componentCount);

			assert(componentCount == 4);

			for(int x = 0; x < imageWidth; x++)
			{
				for(int y = 0; y < imageHeight; y++)
				{
					unsigned char *pixel = sliceData + (y * imageWidth + x) * imageChannels;

					VolumeType::VoxelType voxel;
					pixelToVoxel(pixel, voxel, componentCount);

					// Note: We iterate backwards over y to flip this axis. The images in the VolDat format have x increasing to the right and y
					// increasing downwards. However, we would like our terrain viewed from above (towards negative z) to match the slice images.
					int flippedY = (imageHeight - 1) - y;

					//Might be faster not to use floats here
					volume->setVoxelAt(x, flippedY, slice, voxel, UpdatePriorities::DontUpdate);
				}
			}
		}

		volume->markAsModified(volume->mPolyVoxVolume->getEnclosingRegion(), UpdatePriorities::Background);

		return volume;
	}

	template <typename VolumeType>
	void exportVolDat(VolumeType* volume, std::string folder)
	{
		uint32_t imageWidth = volume->mPolyVoxVolume->getWidth();
		uint32_t imageHeight = volume->mPolyVoxVolume->getHeight();
		uint32_t sliceCount = volume->mPolyVoxVolume->getDepth();
		uint32_t componentCount = 4;

		int outputSliceDataSize = imageWidth * imageHeight * componentCount;
		unsigned char* outputSliceData = new unsigned char[outputSliceDataSize];

		for(uint32_t slice = 0; slice < sliceCount; slice++)
		{
			std::fill(outputSliceData, outputSliceData + imageWidth * imageHeight, 0);

			for(int x = 0; x < imageWidth; x++)
			{
				for(int y = 0; y < imageHeight; y++)
				{
					// Note: We iterate backwards over y to flip this axis. The images in the VolDat format have x increasing to the right and y
					// increasing downwards. However, we would like our terrain viewed from above (towards negative z) to match the slice images.
					int flippedY = (imageHeight - 1) - y;

					unsigned char* pixel = outputSliceData + (y * imageWidth + x) * componentCount;
					VolumeType::VoxelType voxel = volume->mPolyVoxVolume->getVoxel(x, flippedY, slice);

					voxelToPixel(voxel, pixel, componentCount);
				}
			}

			stringstream ss;
			ss << "C:/temp/output/" << setfill('0') << setw(6) << slice << ".png";
			int result = stbi_write_png(ss.str().c_str(), imageWidth, imageHeight, componentCount, outputSliceData, imageWidth * componentCount);
			assert(result); //If crashing here then make sure the output folder exists.
		}

		delete[] outputSliceData;

		//Now write the index file
		/*ofstream indexFile;
		indexFile.open("C:/temp/output/Volume.idx");
		indexFile << "Width = " << imageWidth << endl;
		indexFile << "Height = " << imageHeight << endl;
		indexFile << "SliceCount = " << sliceCount << endl;
		indexFile << "ComponentCount = " << 4 << endl;
		indexFile << "SliceExtension = " << "png" << endl;
		indexFile.close();*/

		FILE *fp;
		fp=fopen("C:/temp/output/Volume.idx", "w");
		fprintf(fp, "Width = %d\n", imageWidth);
		fprintf(fp, "Height = %d\n", imageHeight);
		fprintf(fp, "SliceCount = %d\n", sliceCount);
		fprintf(fp, "ComponentCount = 4\n");
		fprintf(fp, "SliceExtension = png\n");
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
