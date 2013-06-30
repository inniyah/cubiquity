#ifndef GAMEPLAYVOLUME_H_
#define GAMEPLAYVOLUME_H_

#include "gameplay.h"

#include "Volume.h"

#include <sstream>

namespace Cubiquity
{
	template <typename VoxelType>
	class GameplayOctreeNode
	{
	public:
		GameplayOctreeNode(OctreeNode<VoxelType>* octreeNode, GameplayOctreeNode* parent)
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

			std::stringstream ss;
			ss << "LOD = " << int(octreeNode->mHeight) << ", Region = " << octreeNode->mRegion;
			mGameplayNode->setId(ss.str().c_str());
			
			if(parent)
			{
				GP_ASSERT(octreeNode->getParentNode());
				Vector3I translation = octreeNode->mRegion.getLowerCorner() - octreeNode->getParentNode()->mRegion.getLowerCorner();
				mGameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
				parent->mGameplayNode->addChild(mGameplayNode);
			}
			else
			{
				Vector3I translation = octreeNode->mRegion.getLowerCorner();
				mGameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
			}
		}

		~GameplayOctreeNode()
		{
			// Delete any children first
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

			// And remove ourself from gameplay's scenegraph
			if(mParent)
			{
				mParent->mGameplayNode->removeChild(mGameplayNode);
			}
			SAFE_RELEASE(mGameplayNode);
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

		GameplayVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, const char* pageFolder, unsigned int baseNodeSize)
			:mRootGameplayOctreeNode(0)
			,mCubiquityVolume(0)
		{
			mCubiquityVolume = new CubiquityVolumeType(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), pageFolder, baseNodeSize);
		}

		~GameplayVolume()
		{
			delete mCubiquityVolume;
			mCubiquityVolume = 0;
		}

		void initialiseOctree(void);

		gameplay::PhysicsCollisionShape::Definition buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< typename VoxelTraits<typename CubiquityVolumeType::VoxelType>::VertexType>* polyVoxMesh);

		virtual gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< typename VoxelTraits<typename CubiquityVolumeType::VoxelType>::VertexType>* polyVoxMesh) = 0;

		void syncNode(OctreeNode< typename CubiquityVolumeType::VoxelType >* octreeNode, GameplayOctreeNode< typename CubiquityVolumeType::VoxelType >* gameplayOctreeNode);

	protected:

		CubiquityVolumeType* mCubiquityVolume;
		GameplayOctreeNode< typename CubiquityVolumeType::VoxelType >* mRootGameplayOctreeNode;
	};
}

#include "GameplayVolume.inl"

#endif //GAMEPLAYVOLUME_H_