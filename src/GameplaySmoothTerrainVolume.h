#ifndef GAMEPLAYSMOOTHTERRAINVOLUME_H_
#define GAMEPLAYSMOOTHTERRAINVOLUME_H_

#include "SmoothTerrainVolume.h"

#include "gameplay.h"

class GameplaySmoothTerrainVolume : public SmoothTerrainVolume
{
public:
	static GameplaySmoothTerrainVolume* create(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	{
		GameplaySmoothTerrainVolume* volume = new GameplaySmoothTerrainVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);
		return volume;
	}

	void performUpdate(void);

protected:
	GameplaySmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth);
	virtual ~GameplaySmoothTerrainVolume();

	void syncNode(const OctreeNode* octreeNode, gameplay::Node* gameplayNode);

	gameplay::Node* mRootGameplayNode;
};

#endif //GAMEPLAYSMOOTHTERRAINVOLUME_H_
