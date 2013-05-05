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

		void initialiseOctree(void)
		{
			mRootGameplayNode = new GameplayOctreeNode();
			mRootGameplayNode->mGameplayNode = Node::create();

			std::stringstream ss;
			ss << "LOD = " << int(mCubiquityVolume->getRootOctreeNode()->mHeight) << ", Region = (" << mCubiquityVolume->getRootOctreeNode()->mRegion.getLowerX() << "," << mCubiquityVolume->getRootOctreeNode()->mRegion.getLowerY() << "," << mCubiquityVolume->getRootOctreeNode()->mRegion.getLowerZ() << ") to (" << mCubiquityVolume->getRootOctreeNode()->mRegion.getUpperX() << "," << mCubiquityVolume->getRootOctreeNode()->mRegion.getUpperY() << "," << mCubiquityVolume->getRootOctreeNode()->mRegion.getUpperZ() << ")";
			mRootGameplayNode->mGameplayNode->setId(ss.str().c_str());

			Vector3I translation = mCubiquityVolume->getRootOctreeNode()->mRegion.getLowerCorner();
			mRootGameplayNode->mGameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
		}

	protected:

		_VolumeType* mCubiquityVolume;
		GameplayOctreeNode* mRootGameplayNode;
	};

}

#endif //GAMEPLAYVOLUME_H_