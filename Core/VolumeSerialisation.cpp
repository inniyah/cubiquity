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

	void pixelToVoxel(uint8_t* pixelData, Color& voxelData, uint32_t componentCount)
	{
		if(componentCount == 1)
		{
			uint8_t pixel = *pixelData;
			voxelData = Color(255, pixel, 0, pixel > 127 ? 255 : 0);
		}
		else if(componentCount == 4)
		{
			voxelData = Color(*(pixelData + 0), *(pixelData + 1), *(pixelData + 2), *(pixelData + 3));
		}
		else
		{
			POLYVOX_ASSERT(false, "Invalid number of color channels");
		}
	}

	void pixelToVoxel(uint8_t* pixelData, MaterialSet& voxelData, uint32_t componentCount)
	{
		for(uint32_t ct = 0; ct < componentCount; ct++)
		{
			voxelData.setMaterial(ct, *(pixelData + ct));
		}
	}

	void voxelToPixel(Color& voxelData, uint8_t* pixelData, uint32_t componentCount)
	{
		POLYVOX_ASSERT(componentCount == 4, "Images must have four color channels to export from a ColoredCubesVolume");
		*(pixelData + 0) = voxelData.getRed();
		*(pixelData + 1) = voxelData.getGreen();
		*(pixelData + 2) = voxelData.getBlue();
		*(pixelData + 3) = voxelData.getAlpha();
	}

	void voxelToPixel(MaterialSet& voxelData, uint8_t* pixelData, uint32_t componentCount)
	{
		for(uint32_t ct = 0; ct < componentCount; ct++)
		{
			*(pixelData + ct) = static_cast<uint8_t>(voxelData.getMaterial(ct));
		}
	}

	ColoredCubesVolumeImpl* importVxl(const std::string& vxlFilename, const std::string& pathToVoxelDatabase)
	{
		ColoredCubesVolumeImpl* result = new ColoredCubesVolumeImpl(Region(0, 0, 0, 511, 63, 511), pathToVoxelDatabase, 64);

		FILE* inputFile = fopen(vxlFilename.c_str(), "rb");
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
					result->setVoxelAt(x, 63 - zz, y, Color(red, green, blue, 255), UpdatePriorities::DontUpdate);
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
				result->setVoxelAt(x, 63 - zz, y, Color(127, 127, 127, 255), UpdatePriorities::DontUpdate);
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

		result->markAsModified(Region(0, 0, 0, 511, 63, 511), UpdatePriorities::Background);

		return result;
	}

	ColoredCubesVolumeImpl* importHeightmap(const std::string& heightmapFileName, const std::string& colormapFileName, const std::string& pathToVoxelDatabase, uint32_t baseNodeSize)
	{
		int heightmapWidth = 0, heightmapHeight = 0, heightmapChannels;
		unsigned char *heightmapData = stbi_load(heightmapFileName.c_str(), &heightmapWidth, &heightmapHeight, &heightmapChannels, 0);
		POLYVOX_THROW_IF(heightmapData == NULL, ios_base::failure, "Failed to load heightmap");

		int colormapWidth = 0, colormapHeight = 0, colormapChannels;
		unsigned char *colormapData = stbi_load(colormapFileName.c_str(), &colormapWidth, &colormapHeight, &colormapChannels, 0);
		POLYVOX_THROW_IF(colormapData == NULL, ios_base::failure, "Failed to load colormap");
		POLYVOX_THROW_IF(colormapChannels  < 3, std::invalid_argument, "Colormap must be a color image (three or four color channels)");

		POLYVOX_THROW_IF(heightmapWidth != colormapWidth, std::invalid_argument, "Heightmap and colormap must have the same width");
		POLYVOX_THROW_IF(heightmapHeight != colormapHeight, std::invalid_argument, "Heightmap and colormap must have the same height");

		//Create the volume
		int volumeWidth = heightmapWidth;
		int volumeHeight = heightmapHeight;
		int volumeDepth = 256; //Assume 8-bit heightmap

		// When importing we treat 'y' as up because the Gameplay physics engine makes some
		// assumptions about this. This means we need to swap the 'y' and 'slice' indices.
		ColoredCubesVolumeImpl* volume = new ColoredCubesVolumeImpl(Region(0, 0, 0, volumeWidth - 1, volumeDepth - 1, volumeHeight - 1), pathToVoxelDatabase, baseNodeSize);

		// Now iterate over each pixel.
		for(int x = 0; x < volumeWidth; x++)
		{
			for(int y = 0; y < volumeHeight; y++)
			{
				unsigned char *heightmapPixel = heightmapData + (y * heightmapWidth + x) * heightmapChannels;
				unsigned char *colormapPixel = colormapData + (y * colormapWidth + x) * colormapChannels;

				for(int z = 0; z < volumeDepth; z++)
				{
					if(z <= *heightmapPixel)
					{
						unsigned char red = *(colormapPixel);
						unsigned char green = *(colormapPixel+1);
						unsigned char blue = *(colormapPixel+2);
						// Note y and z are swapped again
						volume->setVoxelAt(x, z, y, Color(red, green, blue, 255), UpdatePriorities::DontUpdate);
					}
					else
					{
						// Note y and z are swapped again
						volume->setVoxelAt(x, z, y, Color(0, 0, 0, 0), UpdatePriorities::DontUpdate);
					}
				}
			}
		}

		volume->markAsModified(volume->getEnclosingRegion(), UpdatePriorities::Background);

		return volume;
	}
}