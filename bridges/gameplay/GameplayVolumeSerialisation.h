#ifndef GAMEPLAYVOLUMESERIALISATION_H_
#define GAMEPLAYVOLUMESERIALISATION_H_

#include "VolumeSerialisation.h"

#include "GameplayColouredCubesVolume.h"
#include "GameplaySmoothTerrainVolume.h"

void loadData(const char* filename, GameplayColouredCubesVolume* volume)
{
	loadData(filename, volume->getVolume());
}

void loadData(const char* filename, GameplaySmoothTerrainVolume* volume)
{
	loadData(filename, volume->getVolume());
}

#endif //GAMEPLAYVOLUMESERIALISATION_H_
