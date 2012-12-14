#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include <cassert>

class Colour
{
public:
	Colour() : m_uRed(0), m_uGreen(0), m_uBlue(0), m_uAlpha(0) {}

	bool operator==(const Colour& rhs) const throw()
	{
		return ((m_uRed == rhs.m_uRed) && (m_uGreen == rhs.m_uGreen) && (m_uBlue == rhs.m_uBlue) && (m_uAlpha == rhs.m_uAlpha));
	};

	bool operator!=(const Colour& rhs) const throw()
	{
		return !(*this == rhs);
	}

	uint16_t getRed(void)
	{
		return m_uRed;
	}

	uint16_t getGreen(void)
	{
		return m_uGreen;
	}

	uint16_t getBlue(void)
	{
		return m_uBlue;
	}

	uint16_t getAlpha(void)
	{
		return m_uAlpha;
	}

	void setColour(uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha)
	{
		m_uRed = red;
		m_uGreen = green;
		m_uBlue = blue;
		m_uAlpha = alpha;
	}

public:
	uint16_t m_uRed : 4;
	uint16_t m_uGreen : 4;
	uint16_t m_uBlue : 4;
	uint16_t m_uAlpha : 4;
};

// These operations are used by the smooth raycast to perform trilinear interpolation.
// We never actually do that on this type (because colours are used for cubic surfaces
// not smooth ones) but our use of templates means that this code path still gets compiled.
// The actual implementations simply assert if they are ever called by mistake.
Colour operator+(const Colour& lhs, const Colour& rhs) throw();
Colour operator-(const Colour& lhs, const Colour& rhs) throw();
Colour operator*(const Colour& lhs, float rhs) throw();
Colour operator/(const Colour& lhs, float rhs) throw();

#endif //__COLOUR_H__
