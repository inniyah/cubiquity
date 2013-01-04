#include "CubiquityUtility.h"

using namespace PolyVox;

void rescaleCubicVolume(RawVolume<Colour>* pVolSrc, const Region& regSrc, RawVolume<Colour>* pVolDst, const Region& regDst)
{
	POLYVOX_ASSERT(regSrc.getWidthInVoxels() == regDst.getWidthInVoxels() * 2, "Wrong size!");
	POLYVOX_ASSERT(regSrc.getHeightInVoxels() == regDst.getHeightInVoxels() * 2, "Wrong size!");
	POLYVOX_ASSERT(regSrc.getDepthInVoxels() == regDst.getDepthInVoxels() * 2, "Wrong size!");

	for(uint32_t z = 0; z < regDst.getDepthInVoxels(); z++)
	{
		for(uint32_t y = 0; y < regDst.getHeightInVoxels(); y++)
		{
			for(uint32_t x = 0; x < regDst.getWidthInVoxels(); x++)
			{
				Vector3DInt32 srcPos = regSrc.getLowerCorner() + (Vector3DInt32(x, y, z) * 2);
				Vector3DInt32 dstPos = regDst.getLowerCorner() + Vector3DInt32(x, y, z);
				RawVolume<Colour>::Sampler srcSampler(pVolSrc);
				srcSampler.setPosition(srcPos);

				if(srcSampler.getVoxel().getAlpha() > 0)
				{
					Colour colour;
					colour.setColour(15,15,15,15);
					pVolDst->setVoxelAt(dstPos, colour);
				}
				else
				{
					Colour colour;
					colour.setColour(0,0,0,0);
					pVolDst->setVoxelAt(dstPos, colour);
				}
			}
		}
	}
}

void rescaleCubicVolume(PolyVox::RawVolume<MultiMaterial4>* pVolSrc, const PolyVox::Region& regSrc, PolyVox::RawVolume<MultiMaterial4>* pVolDst, const PolyVox::Region& regDst)
{
	POLYVOX_ASSERT(false, "Not implemented");
}