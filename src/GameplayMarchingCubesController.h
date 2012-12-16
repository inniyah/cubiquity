#ifndef __GameplayMarchingCubesController_H__
#define __GameplayMarchingCubesController_H__

#include <cassert>
#include <limits>

#include "MultiMaterial.h"

#include "PolyVoxCore/BaseVolume.h"
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
class GameplayMarchingCubesController< MultiMaterial4 >
{
public:
	typedef uint8_t DensityType;
	typedef MultiMaterial4 MaterialType;

	GameplayMarchingCubesController(void)
	{
		// Default to a threshold value halfway between the min and max possible values.
		m_tThreshold = MultiMaterial4::getMaxMaterialValue() / 2;
	}

	DensityType convertToDensity(MultiMaterial4 voxel)
	{
		return voxel.getSumOfMaterials();
	}

	MaterialType convertToMaterial(MultiMaterial4 voxel)
	{
		return voxel;
	}

	MaterialType blendMaterials(MultiMaterial4 a, MaterialType b, float weight)
	{
		MultiMaterial4 result;
		for(uint32_t ct = 0; ct < MultiMaterial4::getNoOfMaterials(); ct++)
		{
			float aFloat = static_cast<float>(a.getMaterial(ct));
			float bFloat = static_cast<float>(b.getMaterial(ct));
			float blended = (bFloat - aFloat) * weight + aFloat;
			result.setMaterial(ct, static_cast<uint8_t>(blended));
		}
		return result;
	}

	MultiMaterial4 getBorderValue(void)
	{
		return m_tBorder;
	}

	DensityType getThreshold(void)
	{			
		return m_tThreshold;
	}

	PolyVox::WrapMode getWrapMode(void)
	{
		return m_eWrapMode;
	}

	void setThreshold(DensityType tThreshold)
	{
		m_tThreshold = tThreshold;
	}

	void setWrapMode(PolyVox::WrapMode eWrapMode, MultiMaterial4 tBorder = MultiMaterial4(0))
	{
		m_eWrapMode = eWrapMode;
		m_tBorder = tBorder;
	}

private:
	DensityType m_tThreshold;
	PolyVox::WrapMode m_eWrapMode;
	MultiMaterial4 m_tBorder;
};

// We never use the marching cubes surface extractor with Material16 so this is just a dummy specialisation.
template<>
class GameplayMarchingCubesController< Colour >
{
public:
	typedef float DensityType;
	typedef Colour MaterialType;

	float convertToDensity(Colour voxel)
	{
		assert(false);
		return 0;
	}

	MaterialType convertToMaterial(Colour voxel)
	{
		return MaterialType();
	}

	MaterialType blendMaterials(MaterialType a, MaterialType b, float weight)
	{
		return a;
	}

	Colour getBorderValue(void)
	{
		assert(false);
		return Colour(0);
	}

	float getThreshold(void)
	{
		assert(false);
		return 0;
	}

	PolyVox::WrapMode getWrapMode(void)
	{
		assert(false);
		return PolyVox::WrapModes::Border;
	}

	void setThreshold(DensityType tThreshold)
	{
		assert(false);
	}

	void setWrapMode(PolyVox::WrapMode eWrapMode, Colour tBorder = Colour(0))
	{
		assert(false);
	}
};

#endif //__GameplayMarchingCubesController_H__
