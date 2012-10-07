#include "MultiMaterial.h"

MultiMaterial operator+(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	PolyVox::Vector3DFloat a = lhs.getMaterial();
	PolyVox::Vector3DFloat b = rhs.getMaterial();
	PolyVox::Vector3DFloat result = a + b;
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}

template <typename Type>
MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw()
{
	PolyVox::Vector3DFloat a = lhs.getMaterial();
	PolyVox::Vector3DFloat b = rhs.getMaterial();
	PolyVox::Vector3DFloat result = a - b;
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}

template <typename Type>
MultiMaterial operator/(const MultiMaterial& lhs, uint32_t rhs) throw()
{
	PolyVox::Vector3DFloat a = lhs.getMaterial();
	PolyVox::Vector3DFloat result = a / static_cast<float>(rhs);
	MultiMaterial resultMat;
	resultMat.setMaterial(result);
	return resultMat;
}