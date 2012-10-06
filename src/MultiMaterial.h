#ifndef __MultiMaterial_H__
#define __MultiMaterial_H__

#include "PolyVoxImpl/TypeDef.h"

#include "PolyVoxCore/DefaultIsQuadNeeded.h" //we'll specialise this function for this voxel type
#include "PolyVoxCore/DefaultMarchingCubesController.h" //We'll specialise the controller contained in here

#include "PolyVoxCore/Vector.h"

#include <cassert>

class MultiMaterial
{
public:
	//We expose DensityType and MaterialType in this way so that, when code is
	//templatised on voxel type, it can determine the underlying storage type
	//using code such as 'VoxelType::DensityType value = voxel.getDensity()'
	//or 'VoxelType::MaterialType value = voxel.getMaterial()'.
	typedef float DensityType;
	typedef PolyVox::Vector3DFloat MaterialType;

	MultiMaterial() : m_uMaterial(PolyVox::Vector3DFloat(0, 0, 0)) {}
	MultiMaterial(MaterialType uMaterial) : m_uMaterial(uMaterial) {}

	bool operator==(const MultiMaterial& rhs) const throw()
	{
		return (m_uMaterial == rhs.m_uMaterial);
	};

	bool operator!=(const MultiMaterial& rhs) const throw()
	{
		return !(*this == rhs);
	}

	MaterialType getMaterial() const throw() { return m_uMaterial; }
	void setMaterial(MaterialType uMaterial) { m_uMaterial = uMaterial; }

private:
	MaterialType m_uMaterial;
};

template <>
class PolyVox::DefaultMarchingCubesController< MultiMaterial >
{
public:
	typedef float DensityType;
	typedef PolyVox::Vector3DFloat MaterialType;

	DefaultMarchingCubesController(void)
	{
		// Default to a threshold value halfway between the min and max possible values.
		m_tThreshold = 0.5f;
	}

	DefaultMarchingCubesController(DensityType tThreshold)
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

//This is just a dummy to keep everything compiling
template<>
class PolyVox::DefaultIsQuadNeeded< MultiMaterial >
{
public:
	typedef float DensityType;
	typedef int MaterialType;

	bool operator()(MultiMaterial back, MultiMaterial front, float& materialToUse)
	{
		return false;
	}
};

#endif //__MultiMaterial_H__
