#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "PolyVoxCore/Impl/TypeDef.h"
#include "PolyVoxCore/VertexTypes.h"

#include "BitField.h"

#include <cassert>

namespace Cubiquity
{
	class Color
	{
	public:

		static const uint32_t MaxInOutValue = 255;

		static const uint32_t RedMSB = 31;
		static const uint32_t RedLSB = 28;		
		static const uint32_t GreenMSB = 27;
		static const uint32_t GreenLSB = 24;
		static const uint32_t BlueMSB = 23;
		static const uint32_t BlueLSB = 20;
		static const uint32_t AlphaMSB = 19;
		static const uint32_t AlphaLSB = 16;
		
		static const uint32_t NoOfRedBits = RedMSB - RedLSB + 1;
		static const uint32_t NoOfGreenBits = GreenMSB - GreenLSB + 1;
		static const uint32_t NoOfBlueBits = BlueMSB - BlueLSB + 1;
		static const uint32_t NoOfAlphaBits = AlphaMSB - AlphaLSB + 1;
		
		static const uint32_t RedScaleFactor = MaxInOutValue / ((0x01 << NoOfRedBits) - 1);
		static const uint32_t GreenScaleFactor = MaxInOutValue / ((0x01 << NoOfGreenBits) - 1);
		static const uint32_t BlueScaleFactor = MaxInOutValue / ((0x01 << NoOfBlueBits) - 1);
		static const uint32_t AlphaScaleFactor = MaxInOutValue / ((0x01 << NoOfAlphaBits) - 1);

		Color()
		{
			mChannels.clearAllBits();
		}

		Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = MaxInOutValue)
		{
			mChannels.clearAllBits();
			setColor(red, green, blue, alpha);
		}

		bool operator==(const Color& rhs) const throw()
		{
			return mChannels == rhs.mChannels;
		}

		bool operator!=(const Color& rhs) const throw()
		{
			return !(*this == rhs);
		}

		uint8_t getRed(void)
		{
			return static_cast<uint8_t>(mChannels.getBits(RedMSB, RedLSB) * RedScaleFactor);
		}

		uint8_t getGreen(void)
		{
			return static_cast<uint8_t>(mChannels.getBits(GreenMSB, GreenLSB) * GreenScaleFactor);
		}

		uint8_t getBlue(void)
		{
			return static_cast<uint8_t>(mChannels.getBits(BlueMSB, BlueLSB) * BlueScaleFactor);
		}

		uint8_t getAlpha(void)
		{
			return static_cast<uint8_t>(mChannels.getBits(AlphaMSB, AlphaLSB) * AlphaScaleFactor);
		}

		void setRed(uint8_t value)
		{
			mChannels.setBits(RedMSB, RedLSB, value / RedScaleFactor);
		}

		void setGreen(uint8_t value)
		{
			mChannels.setBits(GreenMSB, GreenLSB, value / GreenScaleFactor);
		}

		void setBlue(uint8_t value)
		{
			mChannels.setBits(BlueMSB, BlueLSB, value / BlueScaleFactor);
		}

		void setAlpha(uint8_t value)
		{
			mChannels.setBits(AlphaMSB, AlphaLSB, value / AlphaScaleFactor);
		}

		void setColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
		{
			setRed(red);
			setGreen(green);
			setBlue(blue);
			setAlpha(alpha);
		}

	private:
		BitField<uint32_t> mChannels;
	};

	// These operations are used by the smooth raycast to perform trilinear interpolation.
	// We never actually do that on this type (because colors are used for cubic surfaces
	// not smooth ones) but our use of templates means that this code path still gets compiled.
	// The actual implementations simply assert if they are ever called by mistake.
	Color operator+(const Color& lhs, const Color& rhs) throw();
	Color operator-(const Color& lhs, const Color& rhs) throw();
	Color operator*(const Color& lhs, float rhs) throw();
	Color operator/(const Color& lhs, float rhs) throw();

	class ColoredCubesIsQuadNeeded
	{
	public:
		bool operator()(Color back, Color front, Color& materialToUse)
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
