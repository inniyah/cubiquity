// CubiquityCTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CubiquityC.h"


int _tmain(int argc, _TCHAR* argv[])
{
	unsigned int volumeID = newSmoothTerrainVolume(0, 0, 0, 31, 31, 31, 32, 32);
	int noOfVerts = getMeshData(volumeID);
	return 0;
}

