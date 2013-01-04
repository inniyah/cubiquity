#include "GameplaySmoothTerrainVolume.h"

GameplaySmoothTerrainVolume::GameplaySmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	:SmoothTerrainVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth)
	,mRootGameplayNode(0)
{
}

GameplaySmoothTerrainVolume::~GameplaySmoothTerrainVolume()
{
}

void GameplaySmoothTerrainVolume::performUpdate(void)
{
	update();

	//Now ensure the gameplay node tree matches the one in the volume.

	if(mRootOctreeNode != 0)
	{
		if(mRootGameplayNode == 0)
		{
			mRootGameplayNode = Node::create();
		}

		syncNode(mRootOctreeNode, mRootGameplayNode);
	}
}

void GameplaySmoothTerrainVolume::syncNode(const OctreeNode* octreeNode, gameplay::Node* gameplayNode)
{
}
