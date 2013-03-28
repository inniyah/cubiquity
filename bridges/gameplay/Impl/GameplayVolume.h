#ifndef GAMEPLAYVOLUME_H_
#define GAMEPLAYVOLUME_H_

#include "gameplay.h"

#include "Volume.h"

#include "Impl/ExtraNodeData.h"

#include <sstream>

namespace Cubiquity
{
	template <typename _VolumeType>
	class GameplayVolume : public gameplay::Ref
	{
	public:
		gameplay::Node* getRootNode(void)
		{
			return mRootGameplayNode;
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
			mCubiquityVolume = new _VolumeType(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize);
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

		void buildNode(OctreeNode<  typename _VolumeType::VoxelType  >* octreeNode, gameplay::Node* gameplayNode);

		_VolumeType* mCubiquityVolume;
		gameplay::Node* mRootGameplayNode;
	};

	template <typename _VolumeType>
	void GameplayVolume<_VolumeType>::buildNode(OctreeNode<  typename _VolumeType::VoxelType  >* octreeNode, gameplay::Node* gameplayNode)
	{
		octreeNode->mGameEngineNode = gameplayNode;

		std::stringstream ss;
		ss << "LOD = " << int(octreeNode->mLodLevel) << ", Region = (" << octreeNode->mRegion.getLowerX() << "," << octreeNode->mRegion.getLowerY() << "," << octreeNode->mRegion.getLowerZ() << ") to (" << octreeNode->mRegion.getUpperX() << "," << octreeNode->mRegion.getUpperY() << "," << octreeNode->mRegion.getUpperZ() << ")";
		gameplayNode->setId(ss.str().c_str());

		if(octreeNode->mParent)
		{
			Vector3I translation = octreeNode->mRegion.getLowerCorner() - octreeNode->mParent->mRegion.getLowerCorner();
			gameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
		}
		else
		{
			Vector3I translation = octreeNode->mRegion.getLowerCorner();
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
							childNode = createNodeWithExtraData< typename _VolumeType::VoxelType >();

							gameplayNode->addChild(childNode);
						}

						buildNode(octreeNode->children[ix][iy][iz], childNode);
					}
				}
			}
		}
	}
}

#endif //GAMEPLAYVOLUME_H_