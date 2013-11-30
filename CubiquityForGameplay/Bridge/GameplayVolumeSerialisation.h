#ifndef GAMEPLAYVOLUMESERIALISATION_H_
#define GAMEPLAYVOLUMESERIALISATION_H_

#include "ColoredCubesVolume.h"
#include "VolumeSerialisation.h"

#include "GameplayColoredCubesVolume.h"
#include "GameplayTerrainVolume.h"

namespace Cubiquity
{
	/**
	 * These functions are wrapped in a class because that seems
	 * to be required for luagen to create bindings for them.
	 */
	class GameplayVolumeSerialisation
	{
	public:
		static void gameplayExportColorSlices(GameplayColoredCubesVolume* volume, const char* voldatFolder)
		{
			exportVolDat<ColoredCubesVolume>(dynamic_cast<ColoredCubesVolume*>(volume->getCubiquityVolume()), voldatFolder);
		}

		static void gameplayExportSmoothSlices(GameplayTerrainVolume* volume, const char* voldatFolder)
		{
			exportVolDat<TerrainVolume>(dynamic_cast<TerrainVolume*>(volume->getCubiquityVolume()), voldatFolder);
		}
	};
}

#endif //GAMEPLAYVOLUMESERIALISATION_H_
