#include "GameplayRaycasting.h"

#include "Raycasting.h"

namespace Cubiquity
{
	bool GameplayRaycasting::gameplayRaycast(GameplayColouredCubesVolume* volume, gameplay::Ray startAndDirection, float length, gameplay::Vector3& result)
	{
		Vector3F v3dStart(startAndDirection.getOrigin().x, startAndDirection.getOrigin().y, startAndDirection.getOrigin().z);
		Vector3F v3dDirection(startAndDirection.getDirection().x, startAndDirection.getDirection().y, startAndDirection.getDirection().z);
		v3dDirection *= length;

		ColouredCubesRaycastTestFunctor raycastTestFunctor;
		::PolyVox::RaycastResult myResult = ::PolyVox::raycastWithDirection(dynamic_cast<ColouredCubesVolumeImpl*>(volume->getCubiquityVolume())->_getPolyVoxVolume(), v3dStart, v3dDirection, raycastTestFunctor);
		if(myResult == ::PolyVox::RaycastResults::Interupted)
		{
			result = gameplay::Vector3(raycastTestFunctor.mLastPos.getX(), raycastTestFunctor.mLastPos.getY(), raycastTestFunctor.mLastPos.getZ());
			return true;
		}

		return false;
	}

	bool GameplayRaycasting::gameplayRaycast(GameplayTerrainVolume* volume, gameplay::Ray startAndDirection, float length, gameplay::Vector3& result)
	{
		Vector3F v3dStart(startAndDirection.getOrigin().x, startAndDirection.getOrigin().y, startAndDirection.getOrigin().z);
		Vector3F v3dDirection(startAndDirection.getDirection().x, startAndDirection.getDirection().y, startAndDirection.getDirection().z);
		v3dDirection *= length;

		RaycastTestFunctor<MultiMaterial> raycastTestFunctor;
		::PolyVox::RaycastResult myResult = terrainRaycastWithDirection(dynamic_cast<TerrainVolumeImpl*>(volume->getCubiquityVolume())->_getPolyVoxVolume(), v3dStart, v3dDirection, raycastTestFunctor, 0.5f);
		if(myResult == ::PolyVox::RaycastResults::Interupted)
		{
			result = gameplay::Vector3(raycastTestFunctor.mLastPos.getX(), raycastTestFunctor.mLastPos.getY(), raycastTestFunctor.mLastPos.getZ());
			return true;
		}

		return false;
	}
}
