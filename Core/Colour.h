#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "PolyVoxCore/Impl/TypeDef.h"
#include "PolyVoxCore/VertexTypes.h"

#include <cassert>

namespace Cubiquity
{
	class Colour
	{
	public:
		Colour() : m_uRed(0), m_uGreen(0), m_uBlue(0), m_uAlpha(0) {}

		Colour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
		{
			setColour(red, green, blue, alpha);
		}

		bool operator==(const Colour& rhs) const throw()
		{
			return ((m_uRed == rhs.m_uRed) && (m_uGreen == rhs.m_uGreen) && (m_uBlue == rhs.m_uBlue) && (m_uAlpha == rhs.m_uAlpha));
		};

		bool operator!=(const Colour& rhs) const throw()
		{
			return !(*this == rhs);
		}

		uint8_t getRed(void)
		{
			return m_uRed << 4;
		}

		uint8_t getGreen(void)
		{
			return m_uGreen << 4;
		}

		uint8_t getBlue(void)
		{
			return m_uBlue << 4;
		}

		uint8_t getAlpha(void)
		{
			return m_uAlpha << 4;
		}

		void setRed(uint8_t value)
		{
			m_uRed = value >> 4;
		}

		void setGreen(uint8_t value)
		{
			m_uGreen = value >> 4;
		}

		void setBlue(uint8_t value)
		{
			m_uBlue = value >> 4;
		}

		void setAlpha(uint8_t value)
		{
			m_uAlpha = value >> 4;
		}

		void setColour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
		{
			setRed(red);
			setGreen(green);
			setBlue(blue);
			setAlpha(alpha);
		}

	private:
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
}

#endif //__COLOUR_H__
