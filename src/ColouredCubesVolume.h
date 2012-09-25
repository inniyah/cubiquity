#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "Volume.h"

#include "PolyVoxCore/Material.h"

class ColouredCubesVolume : public Volume<PolyVox::Material16>
{
public:
	static ColouredCubesVolume* create(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	{
		ColouredCubesVolume* volume = new ColouredCubesVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);

		// We need to add a gap between the regions for the cubic surface extractor
		// as in this case voxels should not be shared between regions (see the cubic
		// surface extractor docs for a diagram). However, we skip this for the upper
		// extremes as we do want to preserve the property of the regions extending
		// outside the volumes (to close off the mesh).
		for(int z = 0; z < volume->mVolumeRegions.getDimension(2); z++)
		{
			for(int y = 0; y < volume->mVolumeRegions.getDimension(1); y++)
			{
				for(int x = 0; x < volume->mVolumeRegions.getDimension(0); x++)
				{
					// For all regions not lying on an upper face of the volume, reduce them by one voxel.
					// There's no need to adjust the noce position as it should match the lower corner, not the upper.
					int xShift = (x < volume->mVolumeRegions.getDimension(0) - 1) ? -1 : 0;
					int yShift = (y < volume->mVolumeRegions.getDimension(1) - 1) ? -1 : 0;
					int zShift = (z < volume->mVolumeRegions.getDimension(2) - 1) ? -1 : 0;
					volume->mVolumeRegions[x][y][z]->mRegion.shiftUpperCorner(Vector3DInt32(xShift, yShift, zShift));
				}
			}
		}

		return volume;
	}

protected:
	ColouredCubesVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
		:Volume<PolyVox::Material16>(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth) {}
};

#endif //COLOUREDCUBESVOLUME_H_
