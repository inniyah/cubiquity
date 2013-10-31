#include "Raycasting.h"
#include "ColouredCubesVolume.h"
#include "TerrainVolume.h"

#include "PolyVoxCore/Picking.h"

namespace Cubiquity
{
	template <typename VoxelType>
	class RaycastTestFunctor
	{
	public:
		RaycastTestFunctor()
		{
		}

		bool operator()(Vector3F pos, const VoxelType& voxel)
		{
		}
	};

	template <>
	class RaycastTestFunctor<MaterialSet>
	{
	public:
		RaycastTestFunctor()
		{
		}

		bool operator()(Vector3F pos, const MaterialSet& voxel)
		{
			mLastPos = pos;
			return voxel.getSumOfMaterials() <= MaterialSet::getMaxMaterialValue() / 2;
		}

		Vector3F mLastPos;
	};

	template <>
	class RaycastTestFunctor<Colour>
	{
	public:
		RaycastTestFunctor()
		{
		}

		bool operator()(Vector3F pos, const Colour& voxel)
		{
			return false;
		}

		Vector3F mLastPos;
	};

	class ColouredCubesRaycastTestFunctor
	{
	public:
		ColouredCubesRaycastTestFunctor()
		{
		}

		bool operator()(const ::PolyVox::POLYVOX_VOLUME<Colour>::Sampler& sampler);

		Vector3I mLastPos;
	};

	bool ColouredCubesRaycastTestFunctor::operator()(const ::PolyVox::POLYVOX_VOLUME<Colour>::Sampler& sampler)
	{
		mLastPos = sampler.getPosition();

		return sampler.getVoxel().getAlpha() == 0;
	}

	bool pickFirstSolidVoxel(ColouredCubesVolume* colouredCubesVolume, float startX, float startY, float startZ, float dirAndLengthX, float dirAndLengthY, float dirAndLengthZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ)
	{
		::PolyVox::Vector3DFloat start(startX, startY, startZ);
		::PolyVox::Vector3DFloat dirAndLength(dirAndLengthX, dirAndLengthY, dirAndLengthZ);

		ColouredCubesVolumeImpl* volumeImpl = dynamic_cast<ColouredCubesVolumeImpl*>(colouredCubesVolume);

		::PolyVox::PickResult result = ::PolyVox::pickVoxel(volumeImpl->_getPolyVoxVolume(), start, dirAndLength, Colour(0, 0, 0, 0));

		if(result.didHit)
		{
			*resultX = result.hitVoxel.getX();
			*resultY = result.hitVoxel.getY();
			*resultZ = result.hitVoxel.getZ();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool pickLastEmptyVoxel(ColouredCubesVolume* colouredCubesVolume, float startX, float startY, float startZ, float dirAndLengthX, float dirAndLengthY, float dirAndLengthZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ)
	{
		::PolyVox::Vector3DFloat start(startX, startY, startZ);
		::PolyVox::Vector3DFloat dirAndLength(dirAndLengthX, dirAndLengthY, dirAndLengthZ);

		ColouredCubesVolumeImpl* volumeImpl = dynamic_cast<ColouredCubesVolumeImpl*>(colouredCubesVolume);

		::PolyVox::PickResult result = ::PolyVox::pickVoxel(volumeImpl->_getPolyVoxVolume(), start, dirAndLength, Colour(0, 0, 0, 0));

		if(result.didHit)
		{
			*resultX = result.previousVoxel.getX();
			*resultY = result.previousVoxel.getY();
			*resultZ = result.previousVoxel.getZ();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool pickTerrainSurface(TerrainVolume* terrainVolume, float startX, float startY, float startZ, float dirAndLengthX, float dirAndLengthY, float dirAndLengthZ, float* resultX, float* resultY, float* resultZ)
	{
		Vector3F v3dStart(startX, startY, startZ);
		Vector3F v3dDirection(dirAndLengthX, dirAndLengthY, dirAndLengthZ);
		//v3dDirection *= length;

		RaycastTestFunctor<MaterialSet> raycastTestFunctor;
		::PolyVox::RaycastResult myResult = terrainRaycastWithDirection(dynamic_cast<TerrainVolumeImpl*>(terrainVolume)->_getPolyVoxVolume(), v3dStart, v3dDirection, raycastTestFunctor, 0.5f);
		if(myResult == ::PolyVox::RaycastResults::Interupted)
		{
			*resultX = raycastTestFunctor.mLastPos.getX();
			*resultY = raycastTestFunctor.mLastPos.getY();
			*resultZ = raycastTestFunctor.mLastPos.getZ();
			return true;
		}

		return false;
	}
}