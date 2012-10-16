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
		m_tThreshold = 127;
	}

	GameplayMarchingCubesController(DensityType tThreshold)
	{
		m_tThreshold = tThreshold;
	}

	DensityType convertToDensity(MultiMaterial voxel)
	{
		return voxel.getMaterial().getX() + voxel.getMaterial().getY() + voxel.getMaterial().getZ() + voxel.getMaterial().getW();
	}

	MaterialType convertToMaterial(MultiMaterial voxel)
	{
		return voxel;
	}

	MaterialType blendMaterials(MaterialType a, MaterialType b, float weight)
	{
		PolyVox::Vector4DFloat aVec = static_cast<PolyVox::Vector4DFloat>(a.getMaterial());
		PolyVox::Vector4DFloat bVec = static_cast<PolyVox::Vector4DFloat>(b.getMaterial());
		PolyVox::Vector4DFloat blendedVec = (bVec - aVec) * weight + aVec;
		MultiMaterial result;
		result.setMaterial(static_cast<PolyVox::Vector4DUint8>(blendedVec));
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
		return PolyVox::Vector4DUint8(0,0,0,0);
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
