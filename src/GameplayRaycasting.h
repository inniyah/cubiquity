#ifndef GAMEPLAYRAYCASTING_H_
#define GAMEPLAYRAYCASTING_H_

#include "SmoothTerrainVolume.h"

bool raycast(SmoothTerrainVolume* volume, gameplay::Ray ray, float distance, gameplay::Vector3& result);

#endif //GAMEPLAYRAYCASTING_H_