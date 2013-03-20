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
	// --------------------------------------------------
	// Converts a string to an integer and returns
	// true only if the conversion was sucessful.
	// --------------------------------------------------
	void convertStringToInt(const std::string& str, int& i)
	{
		// Perform the conversion using strtol().
		char* end;
		i = strtol(str.c_str(), &end, 10);

		// After conversion 'end' should be the null-terminator.
		// If it's something else then the conversion was not a
		// success so we throw an exception.
		if(*end)
		{
			stringstream ss;
			ss << "Could not convert '" << str << "' to an integer";
			throw std::invalid_argument(ss.str());
		}
	}

	// --------------------------------------------------
	// Reads the contens of a VolDat 'Volume.idx'
	// file and return a map with the key and values.
	// --------------------------------------------------
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
		voxelData = Colour(*(pixelData + 0), *(pixelData + 1), *(pixelData + 2), *(pixelData + 3));
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
		POLYVOX_ASSERT(componentCount == 4, "Images must have four colour channels to export from a ColouredCubesVolume");
		*(pixelData + 0) = voxelData.getRed();
		*(pixelData + 1) = voxelData.getGreen();
		*(pixelData + 2) = voxelData.getBlue();
		*(pixelData + 3) = voxelData.getAlpha();
	}

	void voxelToPixel(MultiMaterial& voxelData, uint8_t* pixelData, uint32_t componentCount)
	{
		for(uint32_t ct = 0; ct < componentCount; ct++)
		{
			*(pixelData + ct) = static_cast<uint8_t>(voxelData.getMaterial(ct));
		}
	}

	ColouredCubesVolume* importVxl(const std::string& filename)
	{
		ColouredCubesVolume* result = new ColouredCubesVolume(0, 0, 0, 511, 63, 511, 64, 64);

		FILE* inputFile = fopen(filename.c_str(), "rb");
		POLYVOX_ASSERT(inputFile, "Failed to open input file!");

		// Determine input file's size.
		fseek(inputFile, 0, SEEK_END);
		long fileSize = ftell(inputFile);
		fseek(inputFile, 0, SEEK_SET);

		uint8_t* data = new uint8_t[fileSize];
		long bytesRead = fread(data, sizeof(uint8_t), fileSize, inputFile);
		POLYVOX_ASSERT(fileSize == bytesRead, "Failed to read file!");

		uint8_t N, S, E, A, K, Z, M, colorI, zz, runlength, j, red, green, blue;

		int p;

		int i = 0;
		int x = 0;
		int y = 0;
		int columnI = 0;
		int mapSize = 512;
		int columnCount = mapSize * mapSize;
		while (columnI < columnCount)
		{
			// i = span start byte
			N = data[i];
			S = data[i + 1];
			E = data[i + 2];
			A = data[i + 3];
			K = E - S + 1;
			if (N == 0)
			{
				Z = 0;
				M = 64;
			} else
			{
				Z = (N-1) - K;
				// A of the next span
				M = data[i + N * 4 + 3];
			}
			colorI = 0;
			for (p = 0; p < 2; p++)
			{
				// BEWARE: COORDINATE SYSTEM TRANSFORMATIONS MAY BE NEEDED
				// Get top run of colors
				if (p == 0)
				{
					zz = S;
					runlength = K;
				} else
				{
					// Get bottom run of colors
					zz = M - Z;
					runlength = Z;
				}
				for (j = 0; j < runlength; j++)
				{
					red = data[i + 6 + colorI * 4];
					green = data[i + 5 + colorI * 4];
					blue = data[i + 4 + colorI * 4];
					// Do something with these colors
					//makeVoxelColorful(x, y, zz, red, green, blue);
					result->setVoxelAt(x, 63 - zz, y, Colour(red, green, blue, 255), UpdatePriorities::DontUpdate);
					zz++;
					colorI++;
				}
			}
			// Now deal with solid non-surface voxels
			// No color data is provided for non-surface voxels
			zz = E + 1;
			runlength = M - Z - zz;
			for (j = 0; j < runlength; j++)
			{
				//makeVoxelSolid(x, y, zz);
				result->setVoxelAt(x, 63 - zz, y, Colour(127, 127, 127, 255), UpdatePriorities::DontUpdate);
				zz++;
			}
			if (N == 0)
			{
				columnI++;
				x++;
				if (x >= mapSize)
				{
					x = 0;
					y++;
				}
				i += 4*(1 + K);
			}
			else
			{
				i += N * 4;
			}
		}

		result->markAsModified(::PolyVox::Region(0, 0, 0, 511, 63, 511), UpdatePriorities::Background);

		return result;
	}
}