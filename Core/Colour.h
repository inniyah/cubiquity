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
		static const uint32_t NoOfRedBits = 4;
		static const uint32_t NoOfGreenBits = 4;
		static const uint32_t NoOfBlueBits = 4;
		static const uint32_t NoOfAlphaBits = 4;

		typedef uint16_t StorageType;
		static_assert(NoOfRedBits + NoOfGreenBits + NoOfBlueBits + NoOfAlphaBits <= sizeof(StorageType) * CHAR_BIT, "StorageType does not have enough bits!");

		static const uint32_t MaxInOutValue = 255;
		static const uint32_t RedScaleFactor = MaxInOutValue / ((0x01 << NoOfRedBits) - 1);
		static const uint32_t GreenScaleFactor = MaxInOutValue / ((0x01 << NoOfGreenBits) - 1);
		static const uint32_t BlueScaleFactor = MaxInOutValue / ((0x01 << NoOfBlueBits) - 1);
		static const uint32_t AlphaScaleFactor = MaxInOutValue / ((0x01 << NoOfAlphaBits) - 1);

		Colour() : m_uRed(0), m_uGreen(0), m_uBlue(0), m_uAlpha(0) {}

		Colour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = MaxInOutValue)
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
			return m_uRed * RedScaleFactor;
		}

		uint8_t getGreen(void)
		{
			return m_uGreen * GreenScaleFactor;
		}

		uint8_t getBlue(void)
		{
			return m_uBlue * BlueScaleFactor;
		}

		uint8_t getAlpha(void)
		{
			return m_uAlpha * AlphaScaleFactor;
		}

		void setRed(uint8_t value)
		{
			m_uRed = value / RedScaleFactor;
		}

		void setGreen(uint8_t value)
		{
			m_uGreen = value / GreenScaleFactor;
		}

		void setBlue(uint8_t value)
		{
			m_uBlue = value / BlueScaleFactor;
		}

		void setAlpha(uint8_t value)
		{
			m_uAlpha = value / AlphaScaleFactor;
		}

		void setColour(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
		{
			setRed(red);
			setGreen(green);
			setBlue(blue);
			setAlpha(alpha);
		}

	private:
		StorageType m_uRed : NoOfRedBits;
		StorageType m_uGreen : NoOfGreenBits;
		StorageType m_uBlue : NoOfBlueBits;
		StorageType m_uAlpha : NoOfAlphaBits;
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
