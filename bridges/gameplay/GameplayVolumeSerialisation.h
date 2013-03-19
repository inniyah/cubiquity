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

		/*static GameplayColouredCubesVolume* gameplayImportColourSlices(const char* folderName)
		{
			ColouredCubesVolume* cubiquityVolume = importVolDat<ColouredCubesVolume>(folderName);
			GameplayColouredCubesVolume* result = GameplayColouredCubesVolume::create(cubiquityVolume);
			return result;
		}*/

		static void gameplayExportColourSlices(GameplayColouredCubesVolume* volume, const char* folderName)
		{
			exportVolDat<ColouredCubesVolume>(volume->getCubiquityVolume(), folderName);
		}

		/*static GameplaySmoothTerrainVolume* gameplayImportSmoothSlices(const char* folderName)
		{
			SmoothTerrainVolume* cubiquityVolume = importVolDat<SmoothTerrainVolume>(folderName);
			GameplaySmoothTerrainVolume* result = GameplaySmoothTerrainVolume::create(cubiquityVolume);
			return result;
		}*/

		static void gameplayExportSmoothSlices(GameplaySmoothTerrainVolume* volume, const char* folderName)
		{
			exportVolDat<SmoothTerrainVolume>(volume->getCubiquityVolume(), folderName);
		}

		/*static GameplayColouredCubesVolume* gameplayImportVxl(const char* filename)
		{
			ColouredCubesVolume* cubiquityVolume = importVxl(filename);
			GameplayColouredCubesVolume* result = GameplayColouredCubesVolume::create(cubiquityVolume);
			return result;
		}*/
	};
}

#endif //GAMEPLAYVOLUMESERIALISATION_H_
