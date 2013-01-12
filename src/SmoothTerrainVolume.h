#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "MultiMaterial.h"
#include "Volume.h"

#include "PolyVoxCore/MaterialDensityPair.h"

class SmoothTerrainVolume : public Volume<MultiMaterial4>
{

public:
	SmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
		:Volume<MultiMaterial4>(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth)
	{
	}
};

#endif //SMOOTHTERRAINVOLUME_H_
