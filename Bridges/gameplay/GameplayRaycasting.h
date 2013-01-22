#ifndef GAMEPLAYRAYCASTING_H_
#define GAMEPLAYRAYCASTING_H_

#include "GameplaySmoothTerrainVolume.h"

// These functions are wrapped in a class because that seems
// to be required for luagen to create bingings for them.
class GameplayRaycasting
{
public:

	static bool gameplayRaycast(GameplaySmoothTerrainVolume* volume, gameplay::Ray ray, float distance, gameplay::Vector3& result);
};

#endif //GAMEPLAYRAYCASTING_H_