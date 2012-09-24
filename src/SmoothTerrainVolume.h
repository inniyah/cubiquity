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
		return volume;
	}

protected:
	SmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
		:Volume<PolyVox::Material16>(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth) {}
};

#endif //SMOOTHTERRAINVOLUME_H_
