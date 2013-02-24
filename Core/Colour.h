#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include <cassert>

class Colour
{
public:
	Colour() : m_uRed(0), m_uGreen(0), m_uBlue(0), m_uAlpha(0) {}

	// This is a bit of a hack as PolyVox initialies voxels with VoxelType(0), so we
	// need to be able to be constructed with an int parameter. Is there a better way?
	Colour(int value) : m_uRed(value), m_uGreen(value), m_uBlue(value), m_uAlpha(value) {}
	
	Colour(uint16_t red, uint16_t green, uint16_t blue, uint16_t alpha = 15) : m_uRed(red), m_uGreen(green), m_uBlue(blue), m_uAlpha(alpha) {}

	Colour(float red, float green, float blue, float alpha = 1.0f)
		: m_uRed(static_cast<uint16_t>(red * 15.0f + 0.5f))
		, m_uGreen(static_cast<uint16_t>(green * 15.0f + 0.5f))
		, m_uBlue(static_cast<uint16_t>(blue * 15.0f + 0.5f))
		, m_uAlpha(static_cast<uint16_t>(alpha * 15.0f + 0.5f)) {}

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

	float getRedAsFloat(void)
	{
		return m_uRed / 15.0f;
	}

	float getGreenAsFloat(void)
	{
		return m_uGreen / 15.0f;
	}

	float getBlueAsFloat(void)
	{
		return m_uBlue / 15.0f;
	}

	float getAlphaAsFloat(void)
	{
		return m_uAlpha / 15.0f;
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

class ColouredCubesIsQuadNeeded
{
public:
	bool operator()(Colour back, Colour front, Colour& materialToUse)
	{
		if((back.getAlpha() > 0) && (front.getAlpha() == 0))
		{
			materialToUse = back;
			return true;
		}
		else
		{
			return false;
		}
	}
};

#endif //__COLOUR_H__
