#include "MultiMaterial.h"

MultiMaterial operator+(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	PolyVox::Vector4DFloat a = lhs.getMaterial();
	PolyVox::Vector4DFloat b = rhs.getMaterial();
	PolyVox::Vector4DFloat result = a + b;
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}

MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	PolyVox::Vector4DFloat a = lhs.getMaterial();
	PolyVox::Vector4DFloat b = rhs.getMaterial();
	PolyVox::Vector4DFloat result = a - b;
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}

MultiMaterial operator*(const MultiMaterial& lhs, float rhs) throw()
{
	PolyVox::Vector4DFloat a = lhs.getMaterial();
	PolyVox::Vector4DFloat result = a * rhs;
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}

MultiMaterial operator/(const MultiMaterial& lhs, float rhs) throw()
{
	PolyVox::Vector4DFloat a = lhs.getMaterial();
	PolyVox::Vector4DFloat result = a / rhs;
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}