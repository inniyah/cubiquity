#include "MultiMaterial.h"

#include "PolyVoxCore/Interpolation.h"

namespace PolyVox
{
	MultiMaterial trilerp(
	const MultiMaterial& v000,const MultiMaterial& v100,const MultiMaterial& v010,const MultiMaterial& v110,
	const MultiMaterial& v001,const MultiMaterial& v101,const MultiMaterial& v011,const MultiMaterial& v111,
	const float x, const float y, const float z)
	{
		assert((x >= 0.0f) && (y >= 0.0f) && (z >= 0.0f) && 
			(x <= 1.0f) && (y <= 1.0f) && (z <= 1.0f));

		MultiMaterial tInterpolatedValue;

		for(uint32_t ct = 0; ct < MultiMaterial::getNoOfMaterials(); ct++)
		{
			tInterpolatedValue.setMaterial(ct, static_cast<uint8_t>(trilerp<float>
			(
				static_cast<float>(v000.getMaterial(ct)),
				static_cast<float>(v100.getMaterial(ct)),
				static_cast<float>(v010.getMaterial(ct)),
				static_cast<float>(v110.getMaterial(ct)),
				static_cast<float>(v001.getMaterial(ct)),
				static_cast<float>(v101.getMaterial(ct)),
				static_cast<float>(v011.getMaterial(ct)),
				static_cast<float>(v111.getMaterial(ct)),
				x, y, z
			)));
		}

		return tInterpolatedValue;
	}
}

MultiMaterial operator+(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	MultiMaterial resultMat = lhs;
	resultMat += rhs;
	return resultMat;
}

MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	MultiMaterial resultMat = lhs;
	resultMat -= rhs;
	return resultMat;
}

MultiMaterial operator*(const MultiMaterial& lhs, float rhs) throw()
{
	MultiMaterial resultMat = lhs;
	resultMat *= rhs;
	return resultMat;
}

MultiMaterial operator/(const MultiMaterial& lhs, float rhs) throw()
{
	MultiMaterial resultMat = lhs;
	resultMat /= rhs;
	return resultMat;
}