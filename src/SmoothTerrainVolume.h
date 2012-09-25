#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "Volume.h"

#include "PolyVoxCore/Material.h"

class SmoothTerrainVolume : public Volume<PolyVox::Material16>
{
public:
	static SmoothTerrainVolume* create(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	{
		SmoothTerrainVolume* volume = new SmoothTerrainVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);

		// The default region setup code actually causes the the regions to extend outside the  
		// upper range of the volume. For the Marching cubes this is fine as it ensures the 
		// volume will get closed, so we wnt to mimic this behaviour on the lower edges too.
		for(int z = 0; z < volume->mVolumeRegions.getDimension(2); z++)
		{
			for(int y = 0; y < volume->mVolumeRegions.getDimension(1); y++)
			{
				for(int x = 0; x < volume->mVolumeRegions.getDimension(0); x++)
				{
					// For all regions lying on a lower face of the volume, extend them by one voxel.
					// The lower corner of the voxel is meant to match the node translation so we shiift that as well.
					int xShift = (x == 0) ? -1 : 0;
					int yShift = (y == 0) ? -1 : 0;
					int zShift = (z == 0) ? -1 : 0;
					volume->mVolumeRegions[x][y][z]->mRegion.shiftLowerCorner(Vector3DInt32(xShift, yShift, zShift));
					volume->mVolumeRegions[x][y][z]->mNode->translate(xShift, yShift, zShift);
				}
			}
		}

		return volume;
	}

protected:
	SmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
		:Volume<PolyVox::Material16>(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth) {}
};

#endif //SMOOTHTERRAINVOLUME_H_
