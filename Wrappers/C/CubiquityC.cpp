// CubiquityC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CubiquityC.h"

#include "SmoothTerrainVolume.h"

#include <vector>

std::vector<SmoothTerrainVolume*> gSmoothTerrainVolumes;

// This is an example of an exported function.
CUBIQUITYC_API float getZero(void)
{
	return 0.0f;
}

CUBIQUITYC_API float getOne(void)
{
	return 1.0f;
}

CUBIQUITYC_API unsigned int newSmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
{
	SmoothTerrainVolume* volume = new SmoothTerrainVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize);
	gSmoothTerrainVolumes.push_back(volume);

	int centreX = (lowerX + upperX) / 2;
	int centreY = (lowerY + upperY) / 2;
	int centreZ = (lowerZ + upperZ) / 2;

	MultiMaterial value;
	value.setMaterial(0, 255);
	volume->setVoxelAt(centreX, centreY, centreZ, value, Cubiquity::UpdatePriorities::Immediate);

	return gSmoothTerrainVolumes.size() - 1;
}