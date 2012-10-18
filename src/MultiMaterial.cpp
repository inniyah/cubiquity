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

	for(int ct = 0; ct < tInterpolatedValue.getNoOfMaterials(); ct++)
	{
		tInterpolatedValue.setMaterial(ct, static_cast<uint8_t>(trilinearlyInterpolate<float>(v000.getMaterial(ct), v100.getMaterial(ct), v010.getMaterial(ct), v110.getMaterial(ct), v001.getMaterial(ct), v101.getMaterial(ct), v011.getMaterial(ct), v111.getMaterial(ct), x, y, z)));
	}

	return tInterpolatedValue;
}

MultiMaterial operator+(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	//PolyVox::Vector4DFloat a = static_cast<PolyVox::Vector4DFloat>(lhs.getMaterial());
	//PolyVox::Vector4DFloat b = static_cast<PolyVox::Vector4DFloat>(rhs.getMaterial());
	//PolyVox::Vector4DFloat result = a + b;
	MultiMaterial resultMat;
	resultMat.setMaterial(0, lhs.getMaterial(0) + rhs.getMaterial(0));
	resultMat.setMaterial(1, lhs.getMaterial(1) + rhs.getMaterial(1));
	resultMat.setMaterial(2, lhs.getMaterial(2) + rhs.getMaterial(2));
	resultMat.setMaterial(3, lhs.getMaterial(3) + rhs.getMaterial(3));
	return resultMat;
}

MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	MultiMaterial resultMat;
	resultMat.setMaterial(0, lhs.getMaterial(0) - rhs.getMaterial(0));
	resultMat.setMaterial(1, lhs.getMaterial(1) - rhs.getMaterial(1));
	resultMat.setMaterial(2, lhs.getMaterial(2) - rhs.getMaterial(2));
	resultMat.setMaterial(3, lhs.getMaterial(3) - rhs.getMaterial(3));
	return resultMat;
}

MultiMaterial operator*(const MultiMaterial& lhs, float rhs) throw()
{
	MultiMaterial resultMat;
	resultMat.setMaterial(0, static_cast<uint8_t>(lhs.getMaterial(0) * rhs));
	resultMat.setMaterial(1, static_cast<uint8_t>(lhs.getMaterial(1) * rhs));
	resultMat.setMaterial(2, static_cast<uint8_t>(lhs.getMaterial(2) * rhs));
	resultMat.setMaterial(3, static_cast<uint8_t>(lhs.getMaterial(3) * rhs));
	return resultMat;
}

MultiMaterial operator/(const MultiMaterial& lhs, float rhs) throw()
{
	MultiMaterial resultMat;
	resultMat.setMaterial(0, static_cast<uint8_t>(lhs.getMaterial(0) / rhs));
	resultMat.setMaterial(1, static_cast<uint8_t>(lhs.getMaterial(1) / rhs));
	resultMat.setMaterial(2, static_cast<uint8_t>(lhs.getMaterial(2) / rhs));
	resultMat.setMaterial(3, static_cast<uint8_t>(lhs.getMaterial(3) / rhs));
	return resultMat;
}