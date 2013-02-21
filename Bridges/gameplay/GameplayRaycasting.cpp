#include "GameplayRaycasting.h"

#include "Raycasting.h"

bool GameplayRaycasting::gameplayRaycast(GameplayColouredCubesVolume* volume, gameplay::Ray startAndDirection, float length, gameplay::Vector3& result)
{
	PolyVox::Vector3DFloat v3dStart(startAndDirection.getOrigin().x, startAndDirection.getOrigin().y, startAndDirection.getOrigin().z);
	PolyVox::Vector3DFloat v3dDirection(startAndDirection.getDirection().x, startAndDirection.getDirection().y, startAndDirection.getDirection().z);
	v3dDirection *= length;

	ColouredCubesRaycastTestFunctor raycastTestFunctor;
	PolyVox::RaycastResult myResult = PolyVox::raycastWithDirection(volume->getCubiquityVolume()->mPolyVoxVolume, v3dStart, v3dDirection, raycastTestFunctor);
	if(myResult == PolyVox::RaycastResults::Interupted)
	{
		result = gameplay::Vector3(raycastTestFunctor.mLastPos.getX(), raycastTestFunctor.mLastPos.getY(), raycastTestFunctor.mLastPos.getZ());
		return true;
	}

	return false;
}

bool GameplayRaycasting::gameplayRaycast(GameplaySmoothTerrainVolume* volume, gameplay::Ray startAndDirection, float length, gameplay::Vector3& result)
{
	PolyVox::Vector3DFloat v3dStart(startAndDirection.getOrigin().x, startAndDirection.getOrigin().y, startAndDirection.getOrigin().z);
	PolyVox::Vector3DFloat v3dDirection(startAndDirection.getDirection().x, startAndDirection.getDirection().y, startAndDirection.getDirection().z);
	v3dDirection *= length;

	RaycastTestFunctor<MultiMaterial4> raycastTestFunctor;
	PolyVox::RaycastResult myResult = smoothRaycastWithDirection(volume->getCubiquityVolume()->mPolyVoxVolume, v3dStart, v3dDirection, raycastTestFunctor, 0.5f);
	if(myResult == PolyVox::RaycastResults::Interupted)
	{
		result = gameplay::Vector3(raycastTestFunctor.mLastPos.getX(), raycastTestFunctor.mLastPos.getY(), raycastTestFunctor.mLastPos.getZ());
		return true;
	}

	return false;
}