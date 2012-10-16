#include "MultiMaterial.h"

MultiMaterial operator+(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	PolyVox::Vector4DFloat a = static_cast<PolyVox::Vector4DFloat>(lhs.getMaterial());
	PolyVox::Vector4DFloat b = static_cast<PolyVox::Vector4DFloat>(rhs.getMaterial());
	PolyVox::Vector4DFloat result = a + b;
	MultiMaterial resultMat;
	resultMat.setMaterial(static_cast<PolyVox::Vector4DUint8>(result));
	return resultMat;
}

MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	PolyVox::Vector4DFloat a = static_cast<PolyVox::Vector4DFloat>(lhs.getMaterial());
	PolyVox::Vector4DFloat b = static_cast<PolyVox::Vector4DFloat>(rhs.getMaterial());
	PolyVox::Vector4DFloat result = a - b;
	MultiMaterial resultMat;
	resultMat.setMaterial(static_cast<PolyVox::Vector4DUint8>(result));
	return resultMat;
}

MultiMaterial operator*(const MultiMaterial& lhs, float rhs) throw()
{
	PolyVox::Vector4DFloat a = static_cast<PolyVox::Vector4DFloat>(lhs.getMaterial());
	PolyVox::Vector4DFloat result = a * rhs;
	MultiMaterial resultMat;
	resultMat.setMaterial(static_cast<PolyVox::Vector4DUint8>(result));
	return resultMat;
}

MultiMaterial operator/(const MultiMaterial& lhs, float rhs) throw()
{
	PolyVox::Vector4DFloat a = static_cast<PolyVox::Vector4DFloat>(lhs.getMaterial());
	PolyVox::Vector4DFloat result = a / rhs;
	MultiMaterial resultMat;
	resultMat.setMaterial(static_cast<PolyVox::Vector4DUint8>(result));
	return resultMat;
}