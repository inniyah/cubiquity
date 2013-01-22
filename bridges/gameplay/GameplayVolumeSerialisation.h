#ifndef GAMEPLAYVOLUMESERIALISATION_H_
#define GAMEPLAYVOLUMESERIALISATION_H_

#include "VolumeSerialisation.h"

#include "GameplayColouredCubesVolume.h"
#include "GameplaySmoothTerrainVolume.h"

// These functions are wrapped in a class because that seems
// to be required for luagen to create bingings for them.
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
