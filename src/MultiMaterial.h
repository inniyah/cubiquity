#ifndef __MultiMaterial_H__
#define __MultiMaterial_H__

#include "PolyVoxImpl/TypeDef.h"

#include "PolyVoxCore/Vector.h"

#include <cassert>

class MultiMaterial
{
public:
	MultiMaterial()
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			mMaterials[ct] = 0;
		}
	}

	bool operator==(const MultiMaterial& rhs) const throw()
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			if(mMaterials[ct] != rhs.mMaterials[ct])
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
			float temp = static_cast<float>(mMaterials[ct]);
			float rhsFloat = static_cast<float>(rhs.mMaterials[ct]);
			temp += rhsFloat;
			mMaterials[ct] = static_cast<uint8_t>(temp);
		}
		return *this;
	}

	MultiMaterial& operator-=(const MultiMaterial& rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(mMaterials[ct]);
			float rhsFloat = static_cast<float>(rhs.mMaterials[ct]);
			temp -= rhsFloat;
			mMaterials[ct] = static_cast<uint8_t>(temp);
		}
		return *this;
	}

	MultiMaterial& operator*=(float rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(mMaterials[ct]);
			temp *= rhs;
			mMaterials[ct] = static_cast<uint8_t>(temp);
		}
		return *this;
	}

	MultiMaterial& operator/=(float rhs)
	{
		for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
		{
			float temp = static_cast<float>(mMaterials[ct]);
			temp /= rhs;
			mMaterials[ct] = static_cast<uint8_t>(temp);
		}
		return *this;
	}

	//PolyVox::Vector4DUint8 getMaterial() const throw() { return m_uMaterial; }
	//void setMaterial(PolyVox::Vector4DUint8 uMaterial) { m_uMaterial = uMaterial; }

	uint32_t getNoOfMaterials(void) const
	{
		return 4;
	}

	uint8_t getMaterial(uint32_t id) const
	{
		assert(id < 4);
		return mMaterials[id];
	}

	void setMaterial(uint32_t id, uint8_t value)
	{
		assert(id < 4);
		mMaterials[id] = value;
	}

	uint32_t getSumOfMaterials(void) const
	{
		uint32_t sum = 0;
		for(int ct = 0; ct < getNoOfMaterials(); ct++)
		{
			sum += mMaterials[ct];
		}
		return sum;
	}

public:
	//PolyVox::Vector4DUint8 m_uMaterial;
	uint8_t mMaterials[4];
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
