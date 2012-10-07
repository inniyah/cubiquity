#ifndef __GameplayMarchingCubesController_H__
#define __GameplayMarchingCubesController_H__

#include <cassert>
#include <limits>

#include "MultiMaterial.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/Vector.h"

template<typename VoxelType>
class GameplayMarchingCubesController
{
public:
	typedef VoxelType DensityType;
	typedef float MaterialType;

	GameplayMarchingCubesController(void)
	{
		m_tThreshold = ((std::numeric_limits<DensityType>::min)() + (std::numeric_limits<DensityType>::max)()) / 2;
	}

	GameplayMarchingCubesController(DensityType tThreshold)
	{
		m_tThreshold = tThreshold;
	}

	DensityType convertToDensity(VoxelType voxel)
	{
		return voxel;
	}

	MaterialType convertToMaterial(VoxelType voxel)
	{
		return 1;
	}

	DensityType getThreshold(void)
	{
		return m_tThreshold;
	}

private:
	DensityType m_tThreshold;
};

template <>
class GameplayMarchingCubesController< MultiMaterial >
{
public:
	typedef float DensityType;
	typedef PolyVox::Vector3DFloat MaterialType;

	GameplayMarchingCubesController(void)
	{
		// Default to a threshold value halfway between the min and max possible values.
		m_tThreshold = 0.5f;
	}

	GameplayMarchingCubesController(DensityType tThreshold)
	{
		m_tThreshold = tThreshold;
	}

	DensityType convertToDensity(MultiMaterial voxel)
	{
		return voxel.getMaterial().getX() + voxel.getMaterial().getY() + voxel.getMaterial().getZ();
	}

	MaterialType convertToMaterial(MultiMaterial voxel)
	{
		return voxel.getMaterial();
	}

	MaterialType blendMaterials(MaterialType a, MaterialType b) //Should take some weighting into account here
	{
		/*PolyVox::Vector3DFloat aVec = a.getMaterial();
		PolyVox::Vector3DFloat bVec = b.getMaterial();
		PolyVox::Vector3DFloat blendedVec = aVec + bVec;
		blendedVec = blendedVec / 2.0f;
		MultiMaterial result;
		result.setMaterial(blendedVec);
		return result;*/

		return a;
	}

	DensityType getThreshold(void)
	{			
		return m_tThreshold;
	}

private:
	DensityType m_tThreshold;
};

// We never use the marching cubes surface extractor with Material16 so this is just a dummy specialisation.
template<>
class GameplayMarchingCubesController< PolyVox::Material16 >
{
public:
	typedef float DensityType;
	typedef PolyVox::Vector3DFloat MaterialType;

	float convertToDensity(PolyVox::Material16 voxel)
	{
		assert(false);
		return 0;
	}

	MaterialType convertToMaterial(PolyVox::Material16 voxel)
	{
		assert(false);
		return PolyVox::Vector3DFloat(0,0,0);
	}

	MaterialType blendMaterials(MaterialType a, MaterialType b) //Should take some weighting into account here
	{
		/*PolyVox::Vector3DFloat aVec = a.getMaterial();
		PolyVox::Vector3DFloat bVec = b.getMaterial();
		PolyVox::Vector3DFloat blendedVec = aVec + bVec;
		blendedVec = blendedVec / 2.0f;
		MultiMaterial result;
		result.setMaterial(blendedVec);
		return result;*/

		return a;
	}

	float getThreshold(void)
	{
		assert(false);
		return 0;
	}
};

#endif //__GameplayMarchingCubesController_H__
