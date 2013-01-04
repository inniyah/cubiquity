#ifndef GAMEPLAYSMOOTHTERRAINVOLUME_H_
#define GAMEPLAYSMOOTHTERRAINVOLUME_H_

#include "SmoothTerrainVolume.h"

class GameplaySmoothTerrainVolume : public SmoothTerrainVolume
{
public:
	static GameplaySmoothTerrainVolume* create(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	{
		GameplaySmoothTerrainVolume* volume = new GameplaySmoothTerrainVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);
		return volume;
	}

protected:
	GameplaySmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth);
	virtual ~GameplaySmoothTerrainVolume();
};

#endif //GAMEPLAYSMOOTHTERRAINVOLUME_H_
