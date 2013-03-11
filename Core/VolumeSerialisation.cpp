#include "VolumeSerialisation.h"

#include "ColouredCubesVolume.h"

#include "stb_image.c"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <fstream>

using namespace std;

namespace Cubiquity
{
	bool convertStringToInt(const std::string& str, int& i)
	{
		char* end;
		i = strtol(str.c_str(), &end, 10);
		return (!*end);
	}

	ColouredCubesVolume* importSlices(std::string folder)
	{
		string indexFileName(folder);
		indexFileName = indexFileName + "\\Volume.idx";
		int c;
		FILE* file = fopen(indexFileName.c_str(), "rt");
		char chars[80];
		std::string line;

		std::map<std::string, std::string> index;

		while( fgets(chars,80,file) )
		{
			line = string(chars);

			// Strip white space
			line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

			if(line.at(0) != '#')
			{
				std::stringstream ss(line);
				std::string key;
				std::getline(ss, key, '=');
				std::string value;
				std::getline(ss, value);
				index[key] = value;
			}
		}


		//Open the index file
		/*std::ifstream inputFile;
		//inputFile.open (folder + "/Volume.idx", ifstream::in);
		inputFile.open ("C:\\temp\\Utility.h", ifstream::in);
		assert(inputFile.good());
		assert(inputFile.is_open());
		if(!inputFile.is_open())
		{
			cout << "Error" << std::endl;
		}
		

		std::map<std::string, std::string> index;

		try
		{
			int ch = inputFile.get();
		}
		catch(exception& e)
		{
			cout << e.what();
		}

		while( std::getline( inputFile, line ) )
		{
			// Strip white space
			line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

			if(line.at(0) != '#')
			{
				std::stringstream ss(line);
				std::string key;
				std::getline(ss, key, ':');
				std::string value;
				std::getline(ss, value);
				index[key] = value;
			}
		}*/

		//Create the volume
		int volumeWidth;
		assert(convertStringToInt(index["Width"], volumeWidth));
		int volumeHeight;
		assert(convertStringToInt(index["Height"], volumeHeight));
		int sliceCount;
		assert(convertStringToInt(index["SliceCount"], sliceCount));
		int componentCount;
		assert(convertStringToInt(index["ComponentCount"], componentCount));

		ColouredCubesVolume* volume = new ColouredCubesVolume(0, 0, 0, volumeWidth - 1, volumeHeight - 1, sliceCount - 1, 32, 32);
		for(int slice = 0; slice < sliceCount; slice++)
		{
			std::stringstream ss;
			ss << folder << "\\" << slice << "." << index["SliceExtension"];
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

					//Might be faster not to use floats here
					volume->setVoxelAt(x, y, slice, Colour(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f), UpdatePriorities::DontUpdate);
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

					*red = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, y, slice).getRedAsFloat() * 255.0f);
					*green = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, y, slice).getGreenAsFloat() * 255.0f);
					*blue = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, y, slice).getBlueAsFloat() * 255.0f);
					*alpha = static_cast<unsigned char>(volume->mPolyVoxVolume->getVoxel(x, y, slice).getAlphaAsFloat() * 255.0f);
				}
			}

			stringstream ss;
			ss << "C:/temp/output/" << slice << ".png";
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