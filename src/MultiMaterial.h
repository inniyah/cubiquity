#ifndef __MultiMaterial_H__
#define __MultiMaterial_H__

#include "PolyVoxImpl/TypeDef.h"

#include "PolyVoxCore/Vector.h"

#include <cassert>
#include <limits>

template <typename StorageType, uint32_t BitsPerMaterial, uint32_t NoOfMaterials>
class MultiMaterial
{
public:
	MultiMaterial()
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			setMaterial(ct, 0);
		}
	}

	MultiMaterial(uint32_t value)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			setMaterial(ct, value);
		}
	}

	// This function lets us convert a Vector of floats into a MultiMaterial. 
	// This is useful for performing certain operations with more precision.
	MultiMaterial(const PolyVox::Vector<NoOfMaterials, float>& value) throw()
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			setMaterial(ct, static_cast<uint32_t>(value.getElement(ct) + 0.5f));
		}
	}

	// This function lets us convert a MultiMaterial into a Vector of floats. 
	// This is useful for performing certain operations with more precision.
	operator PolyVox::Vector<NoOfMaterials, float>()
	{
		PolyVox::Vector<NoOfMaterials, float> result;
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			result.setElement(ct, static_cast<float>(getMaterial(ct)));
		}

		return result;
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
			setMaterial(ct, static_cast<uint32_t>(temp));
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
			setMaterial(ct, static_cast<uint32_t>(temp));
		}
		return *this;
	}

	MultiMaterial& operator*=(float rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(getMaterial(ct));
			temp *= rhs;
			setMaterial(ct, static_cast<uint32_t>(temp));
		}
		return *this;
	}

	MultiMaterial& operator/=(float rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(getMaterial(ct));
			temp /= rhs;
			setMaterial(ct, static_cast<uint32_t>(temp));
		}
		return *this;
	}

	static uint32_t getNoOfMaterials(void)
	{
		return NoOfMaterials;
	}

	static uint32_t getMaxMaterialValue(void)
	{
		return (0x01 << BitsPerMaterial) - 1;
	}

	uint32_t getMaterial(uint32_t index) const
	{
		assert(index < getNoOfMaterials());

		// Move the required bits into the least significant bits of result.
		StorageType result = mMaterials >> (BitsPerMaterial * index);

		// Build a mask containing all '0''s except for the least significant bits (which are '1's).
		StorageType mask = std::numeric_limits<StorageType>::max(); //Set to all '1's
		mask = mask << BitsPerMaterial; // Insert the required number of '0's for the lower bits
		mask = ~mask; // And invert
		result = result & mask;

		return static_cast<uint32_t>(result);
	}

	void setMaterial(uint32_t index, uint32_t value)
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
	StorageType mMaterials;
};

template <typename StorageType, uint32_t BitsPerMaterial, uint32_t NoOfMaterials>
MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials> operator+(const MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials>& lhs, const MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials>& rhs) throw()
{
	MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials> resultMat = lhs;
	resultMat += rhs;
	return resultMat;
}

template <typename StorageType, uint32_t BitsPerMaterial, uint32_t NoOfMaterials>
MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials> operator-(const MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials>& lhs, const MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials>& rhs) throw()
{
	MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials> resultMat = lhs;
	resultMat -= rhs;
	return resultMat;
}

template <typename StorageType, uint32_t BitsPerMaterial, uint32_t NoOfMaterials>
MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials> operator*(const MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials>& lhs, float rhs) throw()
{
	MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials> resultMat = lhs;
	resultMat *= rhs;
	return resultMat;
}

template <typename StorageType, uint32_t BitsPerMaterial, uint32_t NoOfMaterials>
MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials> operator/(const MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials>& lhs, float rhs) throw()
{
	MultiMaterial<StorageType, BitsPerMaterial, NoOfMaterials> resultMat = lhs;
	resultMat /= rhs;
	return resultMat;
}

typedef MultiMaterial<uint32_t, 8, 4> MultiMaterial4;

// We overload the trilinear interpolation for the MultiMaterial type because it does not have enough precision.
// The overloaded version converts the values to floats and interpolates those before converting back.
// See also http://www.gotw.ca/publications/mill17.htm - Why Not Specialize Function Templates?
namespace PolyVox
{
	MultiMaterial4 trilinearlyInterpolate(
		const MultiMaterial4& v000,const MultiMaterial4& v100,const MultiMaterial4& v010,const MultiMaterial4& v110,
		const MultiMaterial4& v001,const MultiMaterial4& v101,const MultiMaterial4& v011,const MultiMaterial4& v111,
		const float x, const float y, const float z);
}

#endif //__MultiMaterial_H__
