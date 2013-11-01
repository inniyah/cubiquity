#ifndef GAMEPLAYRAYCASTING_H_
#define GAMEPLAYRAYCASTING_H_

#include "GameplayColoredCubesVolume.h"
#include "GameplayTerrainVolume.h"

namespace Cubiquity
{
	/**
	 * These functions are wrapped in a class because that seems
	 * to be required for luagen to create bingings for them.
	 */
	class GameplayRaycasting
	{
	public:
		static bool pickFirstSolidVoxel(GameplayColoredCubesVolume* volume, gameplay::Ray startAndDirection, float length, gameplay::Vector3& result);
		static bool pickTerrainSurface(GameplayTerrainVolume* volume, gameplay::Ray startAndDirection, float length, gameplay::Vector3& result);
	};
}

#endif //GAMEPLAYRAYCASTING_H_
