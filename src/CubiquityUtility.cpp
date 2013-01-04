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
				//srcSampler.setPosition(srcPos);

				uint32_t noOfSolidVoxels = 0;
				uint32_t totalRed = 0;
				uint32_t totalGreen = 0;
				uint32_t totalBlue = 0;
				uint32_t totalExposedFaces = 0;

				for(uint32_t childZ = 0; childZ < 2; childZ++)
				{
					for(uint32_t childY = 0; childY < 2; childY++)
					{
						for(uint32_t childX = 0; childX < 2; childX++)
						{
							srcSampler.setPosition(srcPos + Vector3DInt32(childX, childY, childZ));

							Colour child = srcSampler.getVoxel();

							if(child.getAlpha () > 0)
							{
								uint32_t exposedFaces = 0;
								if(srcSampler.peekVoxel0px0py1nz().getAlpha() == 0) exposedFaces++;
								if(srcSampler.peekVoxel0px0py1pz().getAlpha() == 0) exposedFaces++;
								if(srcSampler.peekVoxel0px1ny0pz().getAlpha() == 0) exposedFaces++;
								if(srcSampler.peekVoxel0px1py0pz().getAlpha() == 0) exposedFaces++;
								if(srcSampler.peekVoxel1nx0py0pz().getAlpha() == 0) exposedFaces++;
								if(srcSampler.peekVoxel1px0py0pz().getAlpha() == 0) exposedFaces++;

								totalRed += child.getRed() * exposedFaces;
								totalGreen += child.getGreen() * exposedFaces;
								totalBlue += child.getBlue() * exposedFaces;

								totalExposedFaces += exposedFaces;

								noOfSolidVoxels++;
							}							
						}
					}
				}

				if(totalExposedFaces == 0) totalExposedFaces++; //Avoid div by zero

				if(noOfSolidVoxels == 8)
				{
					Colour colour;
					colour.setColour(totalRed / totalExposedFaces, totalGreen / totalExposedFaces, totalBlue / totalExposedFaces, 15);
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