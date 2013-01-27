#ifndef GAMEPLAYVOLUME_H_
#define GAMEPLAYVOLUME_H_

#include "gameplay.h"

#include "Volume.h"

#include "Impl/ExtraNodeData.h"

#include <sstream>

template <typename _VolumeType>
class GameplayVolume : public gameplay::Ref
{
public:
	gameplay::Node* getRootNode(void)
	{
		return mRootGameplayNode;
	}

	//Not sure I like exposing this one... should make some functions/classes friends instead?
	_VolumeType* getVolume(void)
	{
		return mVolume;
	}

protected:
	GameplayVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
		:mRootGameplayNode(0)
	{
		mVolume = new _VolumeType(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize);
	}

	~GameplayVolume()
	{
	}

protected:

	void buildNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode);

	_VolumeType* mVolume;
	gameplay::Node* mRootGameplayNode;
};

template <typename _VolumeType>
void GameplayVolume<_VolumeType>::buildNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode)
{
	octreeNode->mGameEngineNode = gameplayNode;

	std::stringstream ss;
	ss << "LOD = " << int(octreeNode->mLodLevel) << ", Region = (" << octreeNode->mRegion.getLowerX() << "," << octreeNode->mRegion.getLowerY() << "," << octreeNode->mRegion.getLowerZ() << ") to (" << octreeNode->mRegion.getUpperX() << "," << octreeNode->mRegion.getUpperY() << "," << octreeNode->mRegion.getUpperZ() << ")";
	gameplayNode->setId(ss.str().c_str());

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
						childNode = createNodeWithExtraData();

						gameplayNode->addChild(childNode);
					}

					buildNode(octreeNode->children[ix][iy][iz], childNode);
				}
			}
		}
	}
}

#endif //GAMEPLAYVOLUME_H_