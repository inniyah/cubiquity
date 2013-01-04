#include "GameplaySmoothTerrainVolume.h"

GameplaySmoothTerrainVolume::GameplaySmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	:SmoothTerrainVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth)
{
}

GameplaySmoothTerrainVolume::~GameplaySmoothTerrainVolume()
{
}