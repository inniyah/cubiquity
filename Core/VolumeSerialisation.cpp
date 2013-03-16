#include "VolumeSerialisation.h"

#undef STBI_HEADER_FILE_ONLY
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

	void pixelToVoxel(uint8_t* pixelData, Colour& voxelData, uint32_t componentCount)
	{
		POLYVOX_ASSERT(componentCount == 4, "Images must have four colour channels to import into a ColouredCubesVolume");
		voxelData = Colour(*(pixelData + 0) / 255.0f, *(pixelData + 1) / 255.0f, *(pixelData + 2) / 255.0f, *(pixelData + 3) / 255.0f);
	}

	void pixelToVoxel(uint8_t* pixelData, MultiMaterial& voxelData, uint32_t componentCount)
	{
		for(uint32_t ct = 0; ct < componentCount; ct++)
		{
			voxelData.setMaterial(ct, *(pixelData + ct));
		}
	}

	void voxelToPixel(Colour& voxelData, uint8_t* pixelData, uint32_t componentCount)
	{
		uint8_t* red = pixelData;
		uint8_t* green = pixelData + 1;
		uint8_t* blue = pixelData + 2;
		uint8_t* alpha = pixelData + 3;

		*red = static_cast<unsigned char>(voxelData.getRedAsFloat() * 255.0f);
		*green = static_cast<unsigned char>(voxelData.getGreenAsFloat() * 255.0f);
		*blue = static_cast<unsigned char>(voxelData.getBlueAsFloat() * 255.0f);
		*alpha = static_cast<unsigned char>(voxelData.getAlphaAsFloat() * 255.0f);
	}

	void voxelToPixel(MultiMaterial& voxelData, uint8_t* pixelData, uint32_t componentCount)
	{
		uint8_t* red = pixelData;
		uint8_t* green = pixelData + 1;
		uint8_t* blue = pixelData + 2;
		uint8_t* alpha = pixelData + 3;

		*red = static_cast<unsigned char>(voxelData.getMaterial(0));
		*green = static_cast<unsigned char>(voxelData.getMaterial(1));
		*blue = static_cast<unsigned char>(voxelData.getMaterial(2));
		*alpha = static_cast<unsigned char>(voxelData.getMaterial(3));
	}
}