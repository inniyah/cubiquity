#ifndef __MultiMaterial_H__
#define __MultiMaterial_H__

#include "PolyVoxCore/Impl/TypeDef.h"

#include "PolyVoxCore/BaseVolume.h"
#include "PolyVoxCore/Vector.h"
#include "PolyVoxCore/VertexTypes.h"

#include <cassert>
#include <limits>

namespace Cubiquity
{
	class MultiMaterial
	{
	private:
		//These could be template parameters if this class needs to be templatised.
		static const uint32_t NoOfMaterials = 4;
		static const uint32_t BitsPerMaterial = 8;
		typedef uint32_t StorageType;
	public:
		MultiMaterial()
		{
			for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
			{
				setMaterial(ct, 0);
			}
		}

		// This function lets us convert a Vector of floats into a MultiMaterial. 
		// This is useful for performing certain operations with more precision.
		MultiMaterial(const ::PolyVox::Vector<NoOfMaterials, float>& value) throw()
		{
			for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
			{
				setMaterial(ct, static_cast<uint32_t>(value.getElement(ct) + 0.5f));
			}
		}

		// This function lets us convert a MultiMaterial into a Vector of floats. 
		// This is useful for performing certain operations with more precision.
		operator ::PolyVox::Vector<NoOfMaterials, float>()
		{
			::PolyVox::Vector<NoOfMaterials, float> result;
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
			StorageType mask = (std::numeric_limits<StorageType>::max)(); //Set to all '1's
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
			StorageType mask = (std::numeric_limits<StorageType>::max)(); //Set to all '1's
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
			for(uint32_t ct = 0; ct < getNoOfMaterials(); ct++)
			{
				sum += getMaterial(ct);
			}
			return sum;
		}

	public:
		StorageType mMaterials;
	};

	class MultiMaterialMarchingCubesController
	{
	public:
		typedef uint8_t DensityType;
		typedef MultiMaterial MaterialType;

		MultiMaterialMarchingCubesController(void);

		DensityType convertToDensity(MultiMaterial voxel);
		MaterialType convertToMaterial(MultiMaterial voxel);

		MaterialType blendMaterials(MultiMaterial a, MaterialType b, float weight);

		MultiMaterial getBorderValue(void);
		DensityType getThreshold(void);
		::PolyVox::WrapMode getWrapMode(void);

		void setThreshold(DensityType tThreshold);
		void setWrapMode(::PolyVox::WrapMode eWrapMode, MultiMaterial tBorder = MultiMaterial(0));

	private:
		DensityType m_tThreshold;
		::PolyVox::WrapMode m_eWrapMode;
		MultiMaterial m_tBorder;
	};
}

// We overload the trilinear interpolation for the MultiMaterial type because it does not have enough precision.
// The overloaded version converts the values to floats and interpolates those before converting back.
// See also http://www.gotw.ca/publications/mill17.htm - Why Not Specialize Function Templates?
namespace PolyVox
{
	::Cubiquity::MultiMaterial trilerp(
		const ::Cubiquity::MultiMaterial& v000,const ::Cubiquity::MultiMaterial& v100,const ::Cubiquity::MultiMaterial& v010,const ::Cubiquity::MultiMaterial& v110,
		const ::Cubiquity::MultiMaterial& v001,const ::Cubiquity::MultiMaterial& v101,const ::Cubiquity::MultiMaterial& v011,const ::Cubiquity::MultiMaterial& v111,
		const float x, const float y, const float z);
}

#endif //__MultiMaterial_H__
