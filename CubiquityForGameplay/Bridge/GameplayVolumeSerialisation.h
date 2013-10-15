#ifndef GAMEPLAYVOLUMESERIALISATION_H_
#define GAMEPLAYVOLUMESERIALISATION_H_

#include "ColouredCubesVolume.h"
#include "VolumeSerialisation.h"

#include "GameplayColouredCubesVolume.h"
#include "GameplayTerrainVolume.h"

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

		static void gameplayExportSmoothSlices(GameplayTerrainVolume* volume, const char* voldatFolder)
		{
			exportVolDat<TerrainVolumeImpl>(dynamic_cast<TerrainVolumeImpl*>(volume->getCubiquityVolume()), voldatFolder);
		}
	};
}

#endif //GAMEPLAYVOLUMESERIALISATION_H_
