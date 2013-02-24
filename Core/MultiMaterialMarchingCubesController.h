#ifndef __MultiMaterialMarchingCubesController_H__
#define __MultiMaterialMarchingCubesController_H__

#include <cassert>
#include <limits>

#include "Colour.h"
#include "MultiMaterial.h"

#include "PolyVoxCore/BaseVolume.h"

template<typename VoxelType>
class MultiMaterialMarchingCubesController
{
public:
	typedef VoxelType DensityType;
	typedef float MaterialType;

	MultiMaterialMarchingCubesController(void)
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
class MultiMaterialMarchingCubesController< MultiMaterial >
{
public:
	typedef uint8_t DensityType;
	typedef MultiMaterial MaterialType;

	MultiMaterialMarchingCubesController(void)
	{
		// Default to a threshold value halfway between the min and max possible values.
		m_tThreshold = MultiMaterial::getMaxMaterialValue() / 2;
	}

	DensityType convertToDensity(MultiMaterial voxel)
	{
		return voxel.getSumOfMaterials();
	}

	MaterialType convertToMaterial(MultiMaterial voxel)
	{
		return voxel;
	}

	MaterialType blendMaterials(MultiMaterial a, MaterialType b, float weight)
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

	MultiMaterial getBorderValue(void)
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

	void setWrapMode(PolyVox::WrapMode eWrapMode, MultiMaterial tBorder = MultiMaterial(0))
	{
		m_eWrapMode = eWrapMode;
		m_tBorder = tBorder;
	}

private:
	DensityType m_tThreshold;
	PolyVox::WrapMode m_eWrapMode;
	MultiMaterial m_tBorder;
};

// We never use the marching cubes surface extractor with Material16 so this is just a dummy specialisation.
template<>
class MultiMaterialMarchingCubesController< Colour >
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

#endif //__MultiMaterialMarchingCubesController_H__
