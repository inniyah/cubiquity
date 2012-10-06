#ifndef __MultiMaterial_H__
#define __MultiMaterial_H__

#include "PolyVoxImpl/TypeDef.h"

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

#endif //__MultiMaterial_H__
