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

template <typename Type>
MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	PolyVox::Vector4DFloat a = lhs.getMaterial();
	PolyVox::Vector4DFloat b = rhs.getMaterial();
	PolyVox::Vector4DFloat result = a - b;
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}

template <typename Type>
MultiMaterial operator/(const MultiMaterial& lhs, uint32_t rhs) throw()
{
	PolyVox::Vector4DFloat a = lhs.getMaterial();
	PolyVox::Vector4DFloat result = a / static_cast<float>(rhs);
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}