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
		static void gameplayExportColourSlices(GameplayColouredCubesVolume* volume, const char* pageFolder)
		{
			exportVolDat<ColouredCubesVolume>(volume->getCubiquityVolume(), pageFolder);
		}

		static void gameplayExportSmoothSlices(GameplaySmoothTerrainVolume* volume, const char* pageFolder)
		{
			exportVolDat<SmoothTerrainVolume>(volume->getCubiquityVolume(), pageFolder);
		}
	};
}

#endif //GAMEPLAYVOLUMESERIALISATION_H_
