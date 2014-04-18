// CubiquityCTest.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
	#include "stdafx.h"
	#include <Windows.h>
#endif

#include <iostream>
#include <sstream>

#include "CubiquityC.h"

#ifdef UNICODE
typedef std::wostringstream tstringstream;
#else
typedef std::ostringstream tstringstream;
#endif

int main()
{
	//unsigned int volumeID = newColoredCubesVolume(0, 0, 0, 127, 127, 127, 256, 256);
	
	//("C:/code/cubiquity/Examples/Volumes/Version 0/VoxeliensTerrain", 32, &volumeID);
	int width = 128;
	int height = 32;
	int depth = 128;
	unsigned int volumeID;
	//File name should not alreay exist. 32 is the internal chunk size (not imporant).
	cuNewEmptyColoredCubesVolume(0, 0, 0, width - 1, height - 1, depth - 1, "NewVolume.vdb", 32, &volumeID);


	for (int y = 0; y < height; y++)
	{
		std::cout << "Processing slice " << y << std::endl;
		for (int z = 0; z < depth; z++)
		{
			for (int x = 0; x < width; x++)
			{				
				CuColor color;
				if (y < 5)
					color = cuMakeColor(255, 0, 0, 255);
				else if (y < 10)
					color = cuMakeColor(0, 255, 0, 255);
				else if (y < 15)
					color = cuMakeColor(0, 0, 255, 255);

				cuSetVoxel(volumeID, x, y, z, color);
			}
		}
	}

	cuAcceptOverrideBlocks(volumeID);

	// Retieve the stored voxel to check it worked
	CuColor storedColor;
	cuGetVoxel(volumeID, 10, 10, 10, &storedColor);

	// Extract the color components
	uint8_t red, green, blue, alpha;
	cuGetAllComponents(storedColor, &red, &green, &blue, &alpha);

	std::stringstream stros;
	stros << "Stored voxel color is (" << (int)red << ", " << (int)green << ", " << (int)blue << ", " << (int)alpha << ")" << std::endl;

	std::cout << stros.str();

	cuDeleteColoredCubesVolume(volumeID);	

	return 0;
}

