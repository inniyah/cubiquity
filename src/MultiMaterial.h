#ifndef __MultiMaterial_H__
#define __MultiMaterial_H__

#include "PolyVoxImpl/TypeDef.h"

#include "PolyVoxCore/Vector.h"

#include <cassert>

class MultiMaterial
{
public:
	MultiMaterial(float val) : m_uMaterial(PolyVox::Vector4DFloat(val, val, val, val)) {}
	MultiMaterial() : m_uMaterial(PolyVox::Vector4DFloat(0, 0, 0, 0)) {}
	MultiMaterial(PolyVox::Vector4DFloat uMaterial) : m_uMaterial(uMaterial) {}

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
		m_uMaterial += rhs.m_uMaterial;
		return *this;
	}

	MultiMaterial& operator-=(const MultiMaterial& rhs)
	{
		m_uMaterial -= rhs.m_uMaterial;
		return *this;
	}

	MultiMaterial& operator*=(float rhs)
	{
		m_uMaterial *= rhs;
		return *this;
	}

	MultiMaterial& operator/=(float rhs)
	{
		m_uMaterial /= rhs;
		return *this;
	}

	PolyVox::Vector4DFloat getMaterial() const throw() { return m_uMaterial; }
	void setMaterial(PolyVox::Vector4DFloat uMaterial) { m_uMaterial = uMaterial; }

private:
	PolyVox::Vector4DFloat m_uMaterial;
};

MultiMaterial operator+(const MultiMaterial& lhs, const MultiMaterial& rhs) throw();

MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw();

MultiMaterial operator*(const MultiMaterial& lhs, float rhs) throw();

MultiMaterial operator/(const MultiMaterial& lhs, float rhs) throw();

#endif //__MultiMaterial_H__
