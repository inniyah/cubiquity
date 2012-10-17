#include "MultiMaterial.h"

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