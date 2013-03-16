#include "VolumeSerialisation.h"

#include "ColouredCubesVolume.h"
#include "SmoothTerrainVolume.h"

#include "stb_image.c"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>

using namespace std;

namespace Cubiquity
{
	//Converts a string to an integer and returns true only if the conversion was sucessful.
	void convertStringToInt(const std::string& str, int& i)
	{
		//Perform the conversion using strtol().
		char* end;
		i = strtol(str.c_str(), &end, 10);

		// After conversion 'end' should be the null-terminator. If it's something
		// else then the conversion was not a success so we throw an exception.
		if(*end)
		{
			stringstream ss;
			ss << "Could not convert '" << str << "' to an integer";
			throw std::invalid_argument(ss.str());
		}
	}

	std::map<std::string, std::string> parseIndexFile(const std::string& filename)
	{
		std::map<std::string, std::string> keyValuePairs;
		FILE* file = fopen(filename.c_str(), "rt");
		char chars[1024];
		std::string line;

		//Ideally we'd use C++ code like this but it's crashing: http://www.gameplay3d.org/forums/viewtopic.php?f=3&t=378#p2204
		//std::ifstream inputFile;
		//inputFile.open (folder + "/Volume.idx", ifstream::in);
		//while( std::getline( inputFile, line ) )
		//{
		//}

		while( fgets(chars,1024,file) )
		{
			line = string(chars);

			// Strip white space
			line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

			// Lines starting with '#' are comments
			if(line.at(0) != '#')
			{
				std::stringstream ss(line);

				//Everything before the '=' is the key
				std::string key;
				std::getline(ss, key, '=');

				//The rest is the value
				std::string value;
				std::getline(ss, value);

				//Save it in the map
				keyValuePairs[key] = value;
			}
		}

		return keyValuePairs;
	}

	ColouredCubesVolume* importColouredSlices(std::string folder)
	{
		string indexFileName(folder);
		indexFileName = indexFileName + "\\Volume.idx";
		std::map<std::string, std::string> index = parseIndexFile(indexFileName);

		//Create the volume
		int volumeWidth;    convertStringToInt(index["Width"], volumeWidth);
		int volumeHeight;   convertStringToInt(index["Height"], volumeHeight);
		int sliceCount;     convertStringToInt(index["SliceCount"], sliceCount);
		int componentCount; convertStringToInt(index["ComponentCount"], componentCount);

		ColouredCubesVolume* volume = new ColouredCubesVolume(0, 0, 0, volumeWidth - 1, volumeHeight - 1, sliceCount - 1, 32, 32);
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

					unsigned char red = *pixel;
					unsigned char green = *(pixel + 1);
					unsigned char blue = *(pixel + 2);
					unsigned char alpha = *(pixel + 3);

					// Note: We iterate backwards over y to flip this axis. The images in the VolDat format have x increasing to the right and y
					// increasing downwards. However, we would like our terrain viewed from above (towards negative z) to match the slice images.
					int flippedY = (imageHeight - 1) - y;

					//Might be faster not to use floats here
					volume->setVoxelAt(x, flippedY, slice, Colour(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f), UpdatePriorities::DontUpdate);
				}
			}
		}

		volume->markAsModified(volume->mPolyVoxVolume->getEnclosingRegion(), UpdatePriorities::Background);

		return volume;
	}

	void exportSlices(ColouredCubesVolume* volume, std::string folder)
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
					unsigned char* outputSliceDataValue = outputSliceData + (y * imageWidth + x) * componentCount;
					unsigned char* red = outputSliceDataValue;
					unsigned char* green = outputSliceDataValue + 1;
					unsigned char* blue = outputSliceDataValue + 2;
					unsigned char* alpha = outputSliceDataValue + 3;

					// Note: We iterate backwards over y to flip this axis. The images in the VolDat format have x increasing to the right and y
					// increasing downwards. However, we would like our terrain viewed from above (towards negative z) to match the slice images.
					int flippedY = (imageHeight - 1) - y;

					*red = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, flippedY, slice).getRedAsFloat() * 255.0f);
					*green = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, flippedY, slice).getGreenAsFloat() * 255.0f);
					*blue = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, flippedY, slice).getBlueAsFloat() * 255.0f);
					*alpha = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, flippedY, slice).getAlphaAsFloat() * 255.0f);
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

	SmoothTerrainVolume* importSmoothSlices(std::string folder)
	{
		string indexFileName(folder);
		indexFileName = indexFileName + "\\Volume.idx";
		std::map<std::string, std::string> index = parseIndexFile(indexFileName);

		//Create the volume
		int volumeWidth;    convertStringToInt(index["Width"], volumeWidth);
		int volumeHeight;   convertStringToInt(index["Height"], volumeHeight);
		int sliceCount;     convertStringToInt(index["SliceCount"], sliceCount);
		int componentCount; convertStringToInt(index["ComponentCount"], componentCount);

		SmoothTerrainVolume* volume = new SmoothTerrainVolume(0, 0, 0, volumeWidth - 1, volumeHeight - 1, sliceCount - 1, 32, 32);
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

					unsigned char red = *pixel;
					unsigned char green = *(pixel + 1);
					unsigned char blue = *(pixel + 2);
					unsigned char alpha = *(pixel + 3);

					MultiMaterial material;
					material.setMaterial(0, red);
					material.setMaterial(1, green);
					material.setMaterial(2, blue);
					material.setMaterial(3, alpha);

					// Note: We iterate backwards over y to flip this axis. The images in the VolDat format have x increasing to the right and y
					// increasing downwards. However, we would like our terrain viewed from above (towards negative z) to match the slice images.
					int flippedY = (imageHeight - 1) - y;

					volume->setVoxelAt(x, flippedY, slice, material, UpdatePriorities::DontUpdate);
				}
			}
		}

		volume->markAsModified(volume->mPolyVoxVolume->getEnclosingRegion(), UpdatePriorities::Background);

		return volume;
	}

	void exportSlices(SmoothTerrainVolume* volume, std::string folder)
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
					unsigned char* outputSliceDataValue = outputSliceData + (y * imageWidth + x) * componentCount;
					unsigned char* red = outputSliceDataValue;
					unsigned char* green = outputSliceDataValue + 1;
					unsigned char* blue = outputSliceDataValue + 2;
					unsigned char* alpha = outputSliceDataValue + 3;

					// Note: We iterate backwards over y to flip this axis. The images in the VolDat format have x increasing to the right and y
					// increasing downwards. However, we would like our terrain viewed from above (towards negative z) to match the slice images.
					int flippedY = (imageHeight - 1) - y;

					*red = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, flippedY, slice).getMaterial(0));
					*green = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, flippedY, slice).getMaterial(1));
					*blue = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, flippedY, slice).getMaterial(2));
					*alpha = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, flippedY, slice).getMaterial(3));
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
}