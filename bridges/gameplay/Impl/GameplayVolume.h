#ifndef GAMEPLAYVOLUME_H_
#define GAMEPLAYVOLUME_H_

#include "gameplay.h"

#include "Volume.h"

#include <sstream>

namespace Cubiquity
{
	class GameplayOctreeNode
	{
	public:
		GameplayOctreeNode()
			:mGameplayNode(0)
			,mTimeStamp(0)
		{
			for(int iz = 0; iz < 2; iz++)
			{
				for(int iy = 0; iy < 2; iy++)
				{
					for(int ix = 0; ix < 2; ix++)
					{
						mChildren[ix][iy][iz] = 0;
					}
				}
			}
		}

		gameplay::Node* mGameplayNode;
		Timestamp mTimeStamp;

		GameplayOctreeNode* mChildren[2][2][2];
	};

	template <typename _VolumeType>
	class GameplayVolume : public gameplay::Ref
	{
	public:
		gameplay::Node* getRootNode(void)
		{
			return mRootGameplayNode->mGameplayNode;
		}

		//Not sure I like exposing this one... should make some functions/classes friends instead?
		_VolumeType* getCubiquityVolume(void)
		{
			return mCubiquityVolume;
		}

		uint32_t getWidth(void)
		{
			return mCubiquityVolume->getWidth();
		}

		uint32_t getHeight(void)
		{
			return mCubiquityVolume->getHeight();
		}

		uint32_t getDepth(void)
		{
			return mCubiquityVolume->getDepth();
		}

	protected:

		GameplayVolume()
			:mRootGameplayNode(0)
			,mCubiquityVolume(0)
		{
		}

		GameplayVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
			:mRootGameplayNode(0)
		{
			mCubiquityVolume = new _VolumeType(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), blockSize, baseNodeSize);
		}

		GameplayVolume(_VolumeType* cubiquityVolume)
			:mRootGameplayNode(0)
			,mCubiquityVolume(cubiquityVolume) //Consider ownership?
		{
		}

		~GameplayVolume()
		{
		}

	protected:

		void buildNode(OctreeNode<  typename _VolumeType::VoxelType  >* octreeNode, GameplayOctreeNode* gameplayOctreeNode);

		_VolumeType* mCubiquityVolume;
		GameplayOctreeNode* mRootGameplayNode;
	};

	template <typename _VolumeType>
	void GameplayVolume<_VolumeType>::buildNode(OctreeNode<  typename _VolumeType::VoxelType  >* octreeNode, GameplayOctreeNode* gameplayOctreeNode)
	{
		//octreeNode->mGameEngineNode = gameplayNode;

		std::stringstream ss;
		ss << "LOD = " << int(octreeNode->mHeight) << ", Region = (" << octreeNode->mRegion.getLowerX() << "," << octreeNode->mRegion.getLowerY() << "," << octreeNode->mRegion.getLowerZ() << ") to (" << octreeNode->mRegion.getUpperX() << "," << octreeNode->mRegion.getUpperY() << "," << octreeNode->mRegion.getUpperZ() << ")";
		gameplayOctreeNode->mGameplayNode->setId(ss.str().c_str());

		if(octreeNode->getParentNode())
		{
			Vector3I translation = octreeNode->mRegion.getLowerCorner() - octreeNode->getParentNode()->mRegion.getLowerCorner();
			gameplayOctreeNode->mGameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
		}
		else
		{
			Vector3I translation = octreeNode->mRegion.getLowerCorner();
			gameplayOctreeNode->mGameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
		}

		for(int iz = 0; iz < 2; iz++)
		{
			for(int iy = 0; iy < 2; iy++)
			{
				for(int ix = 0; ix < 2; ix++)
				{
					if(octreeNode->getChildNode(ix, iy, iz) != 0)
					{
						GameplayOctreeNode* childGameplayOctreeNode = gameplayOctreeNode->mChildren[ix][iy][iz];
						if(childGameplayOctreeNode == 0)
						{		
							//childGameplayOctreeNode = createNodeWithExtraData< typename _VolumeType::VoxelType >();

							childGameplayOctreeNode = new GameplayOctreeNode();
							childGameplayOctreeNode->mGameplayNode = gameplay::Node::create();

							gameplayOctreeNode->mChildren[ix][iy][iz] = childGameplayOctreeNode;

							gameplayOctreeNode->mGameplayNode->addChild(childGameplayOctreeNode->mGameplayNode);
						}

						buildNode(octreeNode->getChildNode(ix, iy, iz), childGameplayOctreeNode);
					}
				}
			}
		}
	}
}

#endif //GAMEPLAYVOLUME_H_