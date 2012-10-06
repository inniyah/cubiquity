#ifndef __GameplayMarchingCubesController_H__
#define __GameplayMarchingCubesController_H__

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

	DensityType getThreshold(void)
	{			
		return m_tThreshold;
	}

private:
	DensityType m_tThreshold;
};

//This is a dummy so the code can still compile.
template<>
class GameplayMarchingCubesController< PolyVox::Material16 >
{
public:
	typedef float DensityType;
	typedef PolyVox::Vector3DFloat MaterialType;

	float convertToDensity(PolyVox::Material16 voxel)
	{
		return 0;
	}

	MaterialType convertToMaterial(PolyVox::Material16 voxel)
	{
		return PolyVox::Vector3DFloat(0,0,0);
	}

	float getThreshold(void)
	{
		return 0;
	}
};

#endif //__GameplayMarchingCubesController_H__