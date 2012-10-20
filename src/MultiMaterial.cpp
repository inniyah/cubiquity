#include "MultiMaterial.h"

#include "PolyVoxImpl/Utility.h" //Interpolation could probably be brought out of PolyVoxImpl?

using namespace PolyVox;

MultiMaterial trilinearlyInterpolate(
const MultiMaterial& v000,const MultiMaterial& v100,const MultiMaterial& v010,const MultiMaterial& v110,
const MultiMaterial& v001,const MultiMaterial& v101,const MultiMaterial& v011,const MultiMaterial& v111,
const float x, const float y, const float z)
{
	assert((x >= 0.0f) && (y >= 0.0f) && (z >= 0.0f) && 
		(x <= 1.0f) && (y <= 1.0f) && (z <= 1.0f));

	MultiMaterial tInterpolatedValue;

	for(int ct = 0; ct < MultiMaterial::getNoOfMaterials(); ct++)
	{
		tInterpolatedValue.setMaterial(ct, static_cast<uint8_t>(trilinearlyInterpolate<float>(v000.getMaterial(ct), v100.getMaterial(ct), v010.getMaterial(ct), v110.getMaterial(ct), v001.getMaterial(ct), v101.getMaterial(ct), v011.getMaterial(ct), v111.getMaterial(ct), x, y, z)));
	}

	return tInterpolatedValue;
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