#include "Cubiquity.h"

#include "GameplayRaycasting.h"

#include "Raycasting.h"

namespace Cubiquity
{
	bool GameplayRaycasting::pickFirstSolidVoxel(GameplayColouredCubesVolume* volume, gameplay::Ray startAndDirection, float length, gameplay::Vector3& result)
	{
		Vector3F v3dStart(startAndDirection.getOrigin().x, startAndDirection.getOrigin().y, startAndDirection.getOrigin().z);
		Vector3F v3dDirection(startAndDirection.getDirection().x, startAndDirection.getDirection().y, startAndDirection.getDirection().z);
		v3dDirection *= length;

		int32_t resultX, resultY, resultZ;
		bool hit = Cubiquity::pickFirstSolidVoxel(volume->getCubiquityVolume(), v3dStart.getX(), v3dStart.getY(), v3dStart.getZ(), v3dDirection.getX(), v3dDirection.getY(), v3dDirection.getZ(), &resultX, &resultY, &resultZ);

		result.x = resultX;
		result.y = resultY;
		result.z = resultZ;

		return hit;
	}

	bool GameplayRaycasting::pickTerrainSurface(GameplayTerrainVolume* volume, gameplay::Ray startAndDirection, float length, gameplay::Vector3& result)
	{
		Vector3F v3dStart(startAndDirection.getOrigin().x, startAndDirection.getOrigin().y, startAndDirection.getOrigin().z);
		Vector3F v3dDirection(startAndDirection.getDirection().x, startAndDirection.getDirection().y, startAndDirection.getDirection().z);
		v3dDirection *= length;

		float resultX, resultY, resultZ;
		bool hit = Cubiquity::pickTerrainSurface(volume->getCubiquityVolume(), v3dStart.getX(), v3dStart.getY(), v3dStart.getZ(), v3dDirection.getX(), v3dDirection.getY(), v3dDirection.getZ(), &resultX, &resultY, &resultZ);

		result.x = resultX;
		result.y = resultY;
		result.z = resultZ;

		return hit;
	}
}
