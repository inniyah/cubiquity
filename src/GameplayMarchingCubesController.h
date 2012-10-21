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
	typedef uint8_t DensityType;
	typedef MultiMaterial MaterialType;

	GameplayMarchingCubesController(void)
	{
		// Default to a threshold value halfway between the min and max possible values.
		m_tThreshold = MultiMaterial::getMaxMaterialValue() / 2;
	}

	GameplayMarchingCubesController(DensityType tThreshold)
	{
		m_tThreshold = tThreshold;
	}

	DensityType convertToDensity(MultiMaterial voxel)
	{
		return voxel.getSumOfMaterials();
	}

	MaterialType convertToMaterial(MultiMaterial voxel)
	{
		return voxel;
	}

	MaterialType blendMaterials(MaterialType a, MaterialType b, float weight)
	{
		MultiMaterial result;
		for(uint32_t ct = 0; ct < MultiMaterial::getNoOfMaterials(); ct++)
		{
			float aFloat = static_cast<float>(a.getMaterial(ct));
			float bFloat = static_cast<float>(b.getMaterial(ct));
			float blended = (bFloat - aFloat) * weight + aFloat;
			result.setMaterial(ct, static_cast<uint8_t>(blended));
		}
		return result;
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
class GameplayMarchingCubesController< Colour >
{
public:
	typedef float DensityType;
	typedef MultiMaterial MaterialType;

	float convertToDensity(Colour voxel)
	{
		assert(false);
		return 0;
	}

	MaterialType convertToMaterial(Colour voxel)
	{
		assert(false);
		return MaterialType();
	}

	MaterialType blendMaterials(MaterialType a, MaterialType b, float weight)
	{
		return a;
	}

	float getThreshold(void)
	{
		assert(false);
		return 0;
	}
};

#endif //__GameplayMarchingCubesController_H__
