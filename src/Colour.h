#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "PolyVoxCore/DefaultMarchingCubesController.h" //We'll specialise the controller contained in here

#include "PolyVoxImpl/TypeDef.h"

#include <cassert>
#include <limits>

#undef min
#undef max

class Colour
{
public:
	typedef uint16_t DensityType;
	typedef uint16_t MaterialType; //Shouldn't define this one...

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

	uint16_t getMaterial(void)
	{
		return m_uColour;
	}

//private:
	uint16_t m_uColour;
};

Colour operator+(const Colour& lhs, const Colour& rhs) throw();
Colour operator-(const Colour& lhs, const Colour& rhs) throw();
Colour operator*(const Colour& lhs, float rhs) throw();
Colour operator/(const Colour& lhs, float rhs) throw();

/*template<>
class DefaultIsQuadNeeded< Colour >
{
public:
	bool operator()(Colour back, Colour front, float& materialToUse)
	{
		if((back.m_uColour > 0) && (front.m_uColour == 0))
		{
			materialToUse = static_cast<float>(back.m_uColour);
			return true;
		}
		else
		{
			return false;
		}
	}
};

template <>
class DefaultMarchingCubesController< Colour >
{
public:
	typedef uint16_t DensityType;
	typedef uint16_t MaterialType;

	DefaultMarchingCubesController(void)
	{
		// Default to a threshold value halfway between the min and max possible values.
		//m_tThreshold = (Density<Type>::getMinDensity() + Density<Type>::getMaxDensity()) / 2;
	}

	DefaultMarchingCubesController(Colour tThreshold)
	{
		//m_tThreshold = tThreshold;
	}

	DensityType convertToDensity(Colour voxel)
	{
		return 1;
	}

	MaterialType convertToMaterial(Colour voxel)
	{
		return 1;
	}

	DensityType getThreshold(void)
	{			
		return 1;
	}

private:
	//DensityType m_tThreshold;
};*/

#endif //__COLOUR_H__
