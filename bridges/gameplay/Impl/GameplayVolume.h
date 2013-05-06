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
		GameplayOctreeNode(GameplayOctreeNode* parent)
			:mGameplayNode(0)
			,mMeshLastSyncronised(0)
			,mParent(parent)
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

			mGameplayNode = gameplay::Node::create();
			
			if(parent)
			{
				parent->mGameplayNode->addChild(mGameplayNode);
			}
		}

		~GameplayOctreeNode()
		{
			for(int iz = 0; iz < 2; iz++)
			{
				for(int iy = 0; iy < 2; iy++)
				{
					for(int ix = 0; ix < 2; ix++)
					{
						delete mChildren[ix][iy][iz];
						mChildren[ix][iy][iz] = 0;
					}
				}
			}

			mGameplayNode->release();
		}

		gameplay::Node* mGameplayNode;
		Timestamp mMeshLastSyncronised;

		GameplayOctreeNode* mParent;
		GameplayOctreeNode* mChildren[2][2][2];
	};

	template <typename _CubiquityVolumeType>
	class GameplayVolume : public gameplay::Ref
	{
	public:
		typedef _CubiquityVolumeType CubiquityVolumeType;

		gameplay::Node* getRootNode(void)
		{
			return mRootGameplayOctreeNode->mGameplayNode;
		}

		//Not sure I like exposing this one... should make some functions/classes friends instead?
		CubiquityVolumeType* getCubiquityVolume(void)
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
			:mRootGameplayOctreeNode(0)
			,mCubiquityVolume(0)
		{
		}

		GameplayVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
			:mRootGameplayOctreeNode(0)
			,mCubiquityVolume(0)
		{
			mCubiquityVolume = new CubiquityVolumeType(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), blockSize, baseNodeSize);
		}

		~GameplayVolume()
		{
		}

		void initialiseOctree(void);

		gameplay::PhysicsCollisionShape::Definition buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< typename VoxelTraits<typename CubiquityVolumeType::VoxelType>::VertexType>* polyVoxMesh);

		virtual gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< typename VoxelTraits<typename CubiquityVolumeType::VoxelType>::VertexType>* polyVoxMesh) = 0;

		void syncNode(OctreeNode< typename CubiquityVolumeType::VoxelType >* octreeNode, GameplayOctreeNode* gameplayOctreeNode);

	protected:

		CubiquityVolumeType* mCubiquityVolume;
		GameplayOctreeNode* mRootGameplayOctreeNode;
	};
}

#include "GameplayVolume.inl"

#endif //GAMEPLAYVOLUME_H_