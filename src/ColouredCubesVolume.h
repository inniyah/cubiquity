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
		return volume;
	}

protected:
	ColouredCubesVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
		:Volume<PolyVox::Material16>(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth) {}
};

#endif //COLOUREDCUBESVOLUME_H_
