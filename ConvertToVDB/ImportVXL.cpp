#include "ImportVXL.h"

#include "CubiquityC.h"

#include <iostream>

using namespace std;

bool importVxl(const std::string& vxlFilename, const std::string& pathToVoxelDatabase, bool dryRun)
{
	FILE* inputFile = fopen(vxlFilename.c_str(), "rb");
	if(inputFile == NULL)
	{
		cerr << "Failed to open input file!" << endl;
		return false;
	}

	// Determine input file's size.
	fseek(inputFile, 0, SEEK_END);
	long fileSize = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);

	uint8_t* data = new uint8_t[fileSize];
	long bytesRead = fread(data, sizeof(uint8_t), fileSize, inputFile);
	if(fileSize != bytesRead)
	{
		cerr << "Failed to read file!" << endl;
		return false;
	}
	fclose(inputFile);

	uint32_t volumeHandle = 1000000; // Better if handles were ints so they could be set invalid?
	if(!dryRun)
	{
		if(cuNewEmptyColoredCubesVolume(0, 0, 0, 511, 63, 511, pathToVoxelDatabase.c_str(), 32, &volumeHandle) != 1) //FIXME - Hardcoded return value!
		{
			cerr << "Failed to create new empty volume" << endl;
			return false;
		}
	}

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
		// Bounds check before accessing data[...], useful incase we don't actually have a valid VXL file (e.g. in dry-run mode)
		if((i + 3) >= fileSize)
		{
			return false;
		}

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

			// Bounds check before accessing data[...], useful incase we don't actually have a valid VXL file (e.g. in dry-run mode)
			if((i + N * 4 + 3) >= fileSize)
			{
				return false;
			}

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
				// Bounds check before accessing data[...], useful incase we don't actually have a valid VXL file (e.g. in dry-run mode)
				if((i + 6 + colorI * 4) >= fileSize)
				{
					return false;
				}

				if(!dryRun)
				{
					red = data[i + 6 + colorI * 4];
					green = data[i + 5 + colorI * 4];
					blue = data[i + 4 + colorI * 4];
					// Do something with these colors
					//makeVoxelColorful(x, y, zz, red, green, blue);
					CuColor color = cuBuildColor(red, green, blue, 255);
					if(cuSetVoxel(volumeHandle, x, 63 - zz, y, color) != 1) // FIXME - Hardcoded return value.
					{
						cerr << "Error setting voxel color" << endl;
						return false;
					}
				}

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
			if(!dryRun)
			{
				CuColor color = cuBuildColor(127, 127, 127, 255);
				if(cuSetVoxel(volumeHandle, x, 63 - zz, y, color) != 1) // FIXME - Hardcoded return value.
				{
					cerr << "Error setting voxel color" << endl;
					return false;
				}
			}

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

	cuAcceptOverrideBlocks(volumeHandle);
	cuDeleteColoredCubesVolume(volumeHandle);

	return true;
}