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
		static void gameplayExportColourSlices(GameplayColouredCubesVolume* volume, const char* voldatFolder)
		{
			exportVolDat<ColouredCubesVolumeImpl>(dynamic_cast<ColouredCubesVolumeImpl*>(volume->getCubiquityVolume()), voldatFolder);
		}

		static void gameplayExportSmoothSlices(GameplaySmoothTerrainVolume* volume, const char* voldatFolder)
		{
			exportVolDat<SmoothTerrainVolumeImpl>(dynamic_cast<SmoothTerrainVolumeImpl*>(volume->getCubiquityVolume()), voldatFolder);
		}
	};
}

#endif //GAMEPLAYVOLUMESERIALISATION_H_
