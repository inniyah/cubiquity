#ifndef __MultiMaterial_H__
#define __MultiMaterial_H__

#include "PolyVoxImpl/TypeDef.h"

#include "PolyVoxCore/Vector.h"

#include <cassert>

class MultiMaterial
{
public:
	MultiMaterial(uint8_t val) : m_uMaterial(PolyVox::Vector4DUint8(val, val, val, val)) {}
	MultiMaterial() : m_uMaterial(PolyVox::Vector4DUint8(0, 0, 0, 0)) {}
	MultiMaterial(PolyVox::Vector4DUint8 uMaterial) : m_uMaterial(uMaterial) {}

	bool operator==(const MultiMaterial& rhs) const throw()
	{
		return (m_uMaterial == rhs.m_uMaterial);
	};

	bool operator!=(const MultiMaterial& rhs) const throw()
	{
		return !(*this == rhs);
	}

	MultiMaterial& operator+=(const MultiMaterial& rhs)
	{
		PolyVox::Vector4DFloat temp = static_cast<PolyVox::Vector4DFloat>(m_uMaterial);
		PolyVox::Vector4DFloat rhsFloat = static_cast<PolyVox::Vector4DFloat>(rhs.m_uMaterial);
		temp += rhsFloat;
		m_uMaterial = static_cast<PolyVox::Vector4DUint8>(temp);
		return *this;
	}

	MultiMaterial& operator-=(const MultiMaterial& rhs)
	{
		PolyVox::Vector4DFloat temp = static_cast<PolyVox::Vector4DFloat>(m_uMaterial);
		PolyVox::Vector4DFloat rhsFloat = static_cast<PolyVox::Vector4DFloat>(rhs.m_uMaterial);
		temp -= rhsFloat;
		m_uMaterial = static_cast<PolyVox::Vector4DUint8>(temp);
		return *this;
	}

	MultiMaterial& operator*=(float rhs)
	{
		PolyVox::Vector4DFloat temp = static_cast<PolyVox::Vector4DFloat>(m_uMaterial);
		temp *= rhs;
		m_uMaterial = static_cast<PolyVox::Vector4DUint8>(temp);
		return *this;
	}

	MultiMaterial& operator/=(float rhs)
	{
		PolyVox::Vector4DFloat temp = static_cast<PolyVox::Vector4DFloat>(m_uMaterial);
		temp /= rhs;
		m_uMaterial = static_cast<PolyVox::Vector4DUint8>(temp);
		return *this;
	}

	PolyVox::Vector4DUint8 getMaterial() const throw() { return m_uMaterial; }
	void setMaterial(PolyVox::Vector4DUint8 uMaterial) { m_uMaterial = uMaterial; }

	uint8_t getMaterial(uint32_t id) const
	{
		assert(id < 4);
		return m_uMaterial.getElement(id);
	}

	void setMaterial(uint32_t id, uint8_t value)
	{
		assert(id < 4);
		m_uMaterial.setElement(id, value);
	}

	uint32_t getSumOfMaterials(void) const
	{
		return getMaterial().getX() + getMaterial().getY() + getMaterial().getZ() + getMaterial().getW();
	}

public:
	PolyVox::Vector4DUint8 m_uMaterial;
};

MultiMaterial operator+(const MultiMaterial& lhs, const MultiMaterial& rhs) throw();

MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw();

MultiMaterial operator*(const MultiMaterial& lhs, float rhs) throw();

MultiMaterial operator/(const MultiMaterial& lhs, float rhs) throw();

#endif //__MultiMaterial_H__
