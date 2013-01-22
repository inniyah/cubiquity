#include "MultiMaterial.h"

#include "PolyVoxCore/Interpolation.h"

namespace PolyVox
{
	MultiMaterial4 trilerp(
	const MultiMaterial4& v000,const MultiMaterial4& v100,const MultiMaterial4& v010,const MultiMaterial4& v110,
	const MultiMaterial4& v001,const MultiMaterial4& v101,const MultiMaterial4& v011,const MultiMaterial4& v111,
	const float x, const float y, const float z)
	{
		assert((x >= 0.0f) && (y >= 0.0f) && (z >= 0.0f) && 
			(x <= 1.0f) && (y <= 1.0f) && (z <= 1.0f));

		MultiMaterial4 tInterpolatedValue;

		for(int ct = 0; ct < MultiMaterial4::getNoOfMaterials(); ct++)
		{
			tInterpolatedValue.setMaterial(ct, static_cast<uint8_t>(trilerp<float>(v000.getMaterial(ct), v100.getMaterial(ct), v010.getMaterial(ct), v110.getMaterial(ct), v001.getMaterial(ct), v101.getMaterial(ct), v011.getMaterial(ct), v111.getMaterial(ct), x, y, z)));
		}

		return tInterpolatedValue;
	}
}