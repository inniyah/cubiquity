// CubiquityCTest.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
	#include "stdafx.h"
	#include <Windows.h>
#endif

#include <iostream>
#include <sstream>

#include "CubiquityC.h"

void validate(int returnCode)
{
	if (returnCode != CU_OK)
	{
		std::cout << cuGetErrorCodeAsString(returnCode) << " : " << cuGetLastErrorMessage() << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main()
{
	int width = 128;
	int height = 32;
	int depth = 128;
	unsigned int volumeID;
	//File name should not alreay exist. 32 is the internal chunk size (not imporant).
	validate( cuNewEmptyColoredCubesVolume(0, 0, 0, width - 1, height - 1, depth - 1, "NewVolume.vdb", 32, &volumeID) );

	CuColor redCol = cuMakeColor(255, 0, 0, 255);
	CuColor greenCol = cuMakeColor(0, 255, 0, 255);
	CuColor blueCol = cuMakeColor(0, 0, 255, 255);

	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{				
				if (y < 5)
					validate( cuSetVoxel(volumeID, x, y, z, redCol) );
				else if (y < 10)
					validate( cuSetVoxel(volumeID, x, y, z, greenCol) );
				else if (y < 15)
					validate( cuSetVoxel(volumeID, x, y, z, blueCol) );
			}
		}
	}

	validate( cuAcceptOverrideBlocks(volumeID) );

	// Retieve the stored voxel to check it worked
	CuColor storedColor;
	validate( cuGetVoxel(volumeID, 10, 10, 10, &storedColor) );

	// Extract the color components
	uint8_t red, green, blue, alpha;
	cuGetAllComponents(storedColor, &red, &green, &blue, &alpha);

	std::stringstream stros;
	stros << "Stored voxel color is (" << (int)red << ", " << (int)green << ", " << (int)blue << ", " << (int)alpha << ")" << std::endl;

	std::cout << stros.str();

	validate( cuDeleteColoredCubesVolume(volumeID) );

	return 0;
}

