#ifndef GAMEPLAYVOLUMESERIALISATION_H_
#define GAMEPLAYVOLUMESERIALISATION_H_

#include "VolumeSerialisation.h"

#include "GameplayColouredCubesVolume.h"
#include "GameplaySmoothTerrainVolume.h"

class GameplayVolumeSerialisation
{
public:
	static void gameplayLoadData(const char* filename, GameplayColouredCubesVolume* volume)
	{
		loadData(filename, volume->getVolume());
	}

	static void gameplayLoadData(const char* filename, GameplaySmoothTerrainVolume* volume)
	{
		loadData(filename, volume->getVolume());
	}
};

#endif //GAMEPLAYVOLUMESERIALISATION_H_
