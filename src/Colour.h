#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "PolyVoxImpl/TypeDef.h"

#include <cassert>

class Colour
{
public:
	Colour() : m_uColour(0) {}
	Colour(uint16_t uColour) : m_uColour(uColour) {}

	bool operator==(const Colour& rhs) const throw()
	{
		return (m_uColour == rhs.m_uColour);
	};

	bool operator!=(const Colour& rhs) const throw()
	{
		return !(*this == rhs);
	}

	Colour& operator+=(const Colour& rhs)
	{
		//m_uDensity += rhs.m_uDensity;

		return *this;
	}

	Colour& operator-=(const Colour& rhs)
	{
		//m_uDensity += rhs.m_uDensity;

		return *this;
	}

	Colour& operator*=(uint32_t rhs)
	{
		//m_uColour /= rhs;
		return *this;
	}

	Colour& operator/=(uint32_t rhs)
	{
		//m_uColour /= rhs;
		return *this;
	}

	uint16_t getRed(void)
	{
		return m_uColour & 0xF000;
	}

	uint16_t getGreen(void)
	{
		return m_uColour & 0x0F00;
	}

	uint16_t getBlue(void)
	{
		return m_uColour & 0x00F0;
	}

	uint16_t getAlpha(void)
	{
		return m_uColour & 0x000F;
	}

	uint16_t getMaterial(void)
	{
		return m_uColour;
	}

private:
	uint16_t m_uColour;
};

Colour operator+(const Colour& lhs, const Colour& rhs) throw();
Colour operator-(const Colour& lhs, const Colour& rhs) throw();
Colour operator*(const Colour& lhs, float rhs) throw();
Colour operator/(const Colour& lhs, float rhs) throw();

#endif //__COLOUR_H__
