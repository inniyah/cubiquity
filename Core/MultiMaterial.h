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
		static const uint32_t NoOfMaterials = 4;
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
			return 255;
		}

		uint32_t getMaterial(uint32_t index) const
		{
			assert(index < getNoOfMaterials());
			return static_cast<uint32_t>(mMaterials[index]);
		}

		void setMaterial(uint32_t index, uint32_t value)
		{
			assert(index < getNoOfMaterials());

			mMaterials[index] = static_cast<uint8_t>(value);
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

		// This function attempts to adjust the values of the different materials so that they sum to
		// the required value. It does this while attempting to preserve the existing material ratios.
		/*void setSumOfMaterials(uint32_t targetSum)
		{
			uint32_t initialSum = getSumOfMaterials();

			if(initialSum == 0)
			{
			}
			else
			{
				float scaleFactor = static_cast<float>(targetSum) / static_cast<float>(initialSum);
				PolyVox::Vector<NoOfMaterials, float> ideal = *this;
				ideal *= scaleFactor;
				*this = ideal;
			}
		}*/

		void clampSumOfMaterials(void)
		{
			uint32_t initialSum = getSumOfMaterials();
			if(initialSum > getMaxMaterialValue())
			{
				uint32_t excess = initialSum - getMaxMaterialValue();
				uint32_t nextMatToReduce = 0;
				while(excess)
				{
					uint32_t material = getMaterial(nextMatToReduce);

					// Don't reduce if it's aready zero - it will wrap around and become big.
					if(material > 0)
					{
						material--;
						setMaterial(nextMatToReduce, material);

						excess--;
					}

					nextMatToReduce++;
					nextMatToReduce %= NoOfMaterials;
				}
			}

			POLYVOX_ASSERT(getSumOfMaterials() <= getMaxMaterialValue(), "MultiMaterial::clampSum() failed to perform clamping");
		}

	public:
		uint8_t mMaterials[NoOfMaterials];
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
