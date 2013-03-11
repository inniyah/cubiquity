#ifndef GAMEPLAYVOLUMESERIALISATION_H_
#define GAMEPLAYVOLUMESERIALISATION_H_

#include "ColouredCubesVolume.h"
#include "VolumeSerialisation.h"

#include "GameplayColouredCubesVolume.h"
#include "GameplaySmoothTerrainVolume.h"

namespace Cubiquity
{
	/**
	 * These functions are wrapped in a class because that seems
	 * to be required for luagen to create bingings for them.
	 */
	class GameplayVolumeSerialisation
	{
	public:
		static void gameplayLoadData(const char* filename, GameplayColouredCubesVolume* volume)
		{
			loadData(filename, volume->getCubiquityVolume());
		}

		static void gameplayLoadData(const char* filename, GameplaySmoothTerrainVolume* volume)
		{
			loadData(filename, volume->getCubiquityVolume());
		}

		static GameplayColouredCubesVolume* gameplayImportColourSlices(const char* folderName)
		{
			ColouredCubesVolume* cubiquityVolume = importSlices(folderName);
			GameplayColouredCubesVolume* result = GameplayColouredCubesVolume::create(cubiquityVolume);
			return result;
		}
	};
}

#endif //GAMEPLAYVOLUMESERIALISATION_H_
