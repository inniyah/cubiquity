#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "MultiMaterial.h"
#include "Volume.h"

#include "PolyVoxCore/MaterialDensityPair.h"

void copyVolume(const SimpleVolume<MultiMaterial4>* src, SimpleVolume<MultiMaterial4>* dst)
{
	Region reg = src->getEnclosingRegion();
	int minX = reg.getLowerCorner().getX();
	int minY = reg.getLowerCorner().getY();
	int minZ = reg.getLowerCorner().getZ();
	int maxX = reg.getUpperCorner().getX();
	int maxY = reg.getUpperCorner().getY();
	int maxZ = reg.getUpperCorner().getZ();
	
	for(int z = minZ; z <= maxZ; z++)
	{
		for(int y = minY; y <= maxY; y++)
		{
			for(int x = minX; x <= maxX; x++)
			{
				dst->setVoxelAt(x,y,z,src->getVoxelAt(x,y,z));

				/*float targetLength = basedOn->getVoxelAt(x,y,z).getMaterial().length();
				float currentLength = toNormalise->getVoxelAt(x,y,z).getMaterial().length();
				float multiplier = targetLength / currentLength;

				if(currentLength < 0.001)
				{
					toNormalise->setVoxelAt(x,y,z,basedOn->getVoxelAt(x,y,z));
				}
				else
				{
				Vector3DFloat values = toNormalise->getVoxelAt(x,y,z).getMaterial();
				values *= multiplier;
				float resultLength = values.length();
				assert(abs(targetLength - resultLength) < 0.001);
				MultiMaterial value;
				value.setMaterial(values);
				toNormalise->setVoxelAt(x,y,z,value);
				}*/
			}
		}
	}
}

class SmoothTerrainVolume : public Volume<MultiMaterial4>
{
public:
	static SmoothTerrainVolume* create(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	{
		SmoothTerrainVolume* volume = new SmoothTerrainVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);
		return volume;
	}

protected:
	SmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
		:Volume<MultiMaterial4>(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth) {}
};

#endif //SMOOTHTERRAINVOLUME_H_
