#ifndef GAMEPLAYVOLUME_H_
#define GAMEPLAYVOLUME_H_

#include "gameplay.h"

#include "Volume.h"

#include "ExtraNodeData.h"

template <typename _VolumeType>
class GameplayVolume : public gameplay::Ref
{
protected:
	GameplayVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth, unsigned int baseNodeSize)
		:mRootGameplayNode(0)
	{
		mVolume = new _VolumeType(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth, baseNodeSize);
	}

	~GameplayVolume()
	{
	}

	void buildNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode);

public:

	_VolumeType* mVolume;

	gameplay::Node* mRootGameplayNode;
};

template <typename _VolumeType>
void GameplayVolume<_VolumeType>::buildNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode)
{
	octreeNode->mGameEngineNode = gameplayNode;

	if(octreeNode->parent)
	{
		PolyVox::Vector3DInt32 translation = octreeNode->mRegion.getLowerCorner() - octreeNode->parent->mRegion.getLowerCorner();
		gameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
	}
	else
	{
		PolyVox::Vector3DInt32 translation = octreeNode->mRegion.getLowerCorner();
		gameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
	}

	for(int iz = 0; iz < 2; iz++)
	{
		for(int iy = 0; iy < 2; iy++)
		{
			for(int ix = 0; ix < 2; ix++)
			{
				if(octreeNode->children[ix][iy][iz] != 0)
				{
					gameplay::Node* childNode = reinterpret_cast<gameplay::Node*>(octreeNode->children[ix][iy][iz]->mGameEngineNode);
					if(childNode == 0)
					{
						childNode = createNodeWithExtraData("ChildGameplayNode");

						gameplayNode->addChild(childNode);
					}

					buildNode(octreeNode->children[ix][iy][iz], childNode);
				}
			}
		}
	}
}

#endif //GAMEPLAYVOLUME_H_