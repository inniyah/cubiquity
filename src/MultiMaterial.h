#ifndef __MultiMaterial_H__
#define __MultiMaterial_H__

#include "PolyVoxImpl/TypeDef.h"

#include "PolyVoxCore/Vector.h"

#include <cassert>
#include <limits>

class MultiMaterial
{
	typedef uint32_t StorageType;
	static const uint8_t BitsPerMaterial = 8;
	static const uint8_t NoOfMaterials = 4;
	static const uint8_t MaxMaterialValue = 255;

public:
	MultiMaterial()
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			setMaterial(ct, 0);
		}
	}

	bool operator==(const MultiMaterial& rhs) const throw()
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			if(getMaterial(ct) != rhs.getMaterial(ct))
			{
				return false;
			}
		}
		return true;
	};

	bool operator!=(const MultiMaterial& rhs) const throw()
	{
		return !(*this == rhs);
	}

	MultiMaterial& operator+=(const MultiMaterial& rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(getMaterial(ct));
			float rhsFloat = static_cast<float>(rhs.getMaterial(ct));
			temp += rhsFloat;
			setMaterial(ct, static_cast<uint8_t>(temp));
		}
		return *this;
	}

	MultiMaterial& operator-=(const MultiMaterial& rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(getMaterial(ct));
			float rhsFloat = static_cast<float>(rhs.getMaterial(ct));
			temp -= rhsFloat;
			setMaterial(ct, static_cast<uint8_t>(temp));
		}
		return *this;
	}

	MultiMaterial& operator*=(float rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(getMaterial(ct));
			temp *= rhs;
			setMaterial(ct, static_cast<uint8_t>(temp));
		}
		return *this;
	}

	MultiMaterial& operator/=(float rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(getMaterial(ct));
			temp /= rhs;
			setMaterial(ct, static_cast<uint8_t>(temp));
		}
		return *this;
	}

	static uint32_t getNoOfMaterials(void)
	{
		return NoOfMaterials;
	}

	static uint8_t getMaxMaterialValue(void)
	{
		return MaxMaterialValue;
	}

	uint8_t getMaterial(uint32_t index) const
	{
		assert(index < getNoOfMaterials());

		// Move the required bits into the least significant bits of result.
		StorageType result = mMaterials >> (BitsPerMaterial * index);

		// Build a mask containing all '0''s except for the least significant bits (which are '1's).
		StorageType mask = std::numeric_limits<StorageType>::max(); //Set to all '1's
		mask = mask << BitsPerMaterial; // Insert the required number of '0's for the lower bits
		mask = ~mask; // And invert
		result = result & mask;

		return static_cast<uint8_t>(result);
	}

	void setMaterial(uint32_t index, uint8_t value)
	{
		assert(index < getNoOfMaterials());

		// The bits we want to set first get cleared to zeros.
		// To do this we create a mask which is all '1' except
		// for the bits we wish to clear (which are '0').
		StorageType mask = std::numeric_limits<StorageType>::max(); //Set to all '1's
		mask = mask << BitsPerMaterial; // Insert the required number of '0's for the lower bits
		mask = ~mask; // We want to insert '1's next, so fake this by inverting before and after
		mask = mask << (BitsPerMaterial * index); // Insert the '0's which we will invert to '1's.
		mask = ~mask; // And invert back again

		// Clear the bits which we're about to set.
		mMaterials &= mask;

		// OR with the value to set the bits
		StorageType temp = value;
		temp = temp << (BitsPerMaterial * index);
		mMaterials |= temp;
	}

	uint32_t getSumOfMaterials(void) const
	{
		uint32_t sum = 0;
		for(int ct = 0; ct < getNoOfMaterials(); ct++)
		{
			sum += getMaterial(ct);
		}
		return sum;
	}

public:
	//uint8_t mMaterials[NoOfMaterials];
	StorageType mMaterials;
};

MultiMaterial operator+(const MultiMaterial& lhs, const MultiMaterial& rhs) throw();

MultiMaterial operator-(const MultiMaterial& lhs, const MultiMaterial& rhs) throw();

MultiMaterial operator*(const MultiMaterial& lhs, float rhs) throw();

MultiMaterial operator/(const MultiMaterial& lhs, float rhs) throw();

// We overload the trilinear interpolation for the MultiMaterial type because it does not have enough precision.
// The overloaded version converts the values to floats and interpolates those before converting back.
// See also http://www.gotw.ca/publications/mill17.htm - Why Not Specialize Function Templates?
MultiMaterial trilinearlyInterpolate(
const MultiMaterial& v000,const MultiMaterial& v100,const MultiMaterial& v010,const MultiMaterial& v110,
const MultiMaterial& v001,const MultiMaterial& v101,const MultiMaterial& v011,const MultiMaterial& v111,
const float x, const float y, const float z);

#endif //__MultiMaterial_H__
