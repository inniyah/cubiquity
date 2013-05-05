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

		void syncNode(OctreeNode< typename CubiquityVolumeType::VoxelType >* octreeNode, GameplayOctreeNode* gameplayOctreeNode)
		{
			if(gameplayOctreeNode->mTimeStamp < octreeNode->mMeshLastUpdated)
			{
				if(octreeNode->mPolyVoxMesh)
				{
					Model* model = buildModelFromPolyVoxMesh(octreeNode->mPolyVoxMesh);
					if(VoxelTraits<typename CubiquityVolumeType::VoxelType>::IsColour)
					{
						model->setMaterial("res/Materials/ColouredCubicTerrain.material");
					}
					else
					{
						model->setMaterial("res/Materials/SmoothTerrain.material");
					}
					gameplayOctreeNode->mGameplayNode->setModel(model);
					SAFE_RELEASE(model);

					//There is a weird bug, whereby if we use the LOD 0 for physics it resets the node positions somehow. So we use LOD 1 here.
					//if(octreeNode->mHeight == 0)
					{
						PhysicsCollisionShape::Definition physDef = buildCollisionObjectFromPolyVoxMesh(octreeNode->mPolyVoxMesh);

						PhysicsRigidBody::Parameters groundParams;
						groundParams.mass = 0.0f;
						gameplayOctreeNode->mGameplayNode->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, physDef, &groundParams);
					}
				}	

				gameplayOctreeNode->mTimeStamp = Clock::getTimestamp();
			}

			if(octreeNode->mRenderThisNode)
			{
				gameplayOctreeNode->mGameplayNode->setTag("RenderThisNode", "t");
			}
			else
			{
				gameplayOctreeNode->mGameplayNode->setTag("RenderThisNode", "f");
			}

			for(int iz = 0; iz < 2; iz++)
			{
				for(int iy = 0; iy < 2; iy++)
				{
					for(int ix = 0; ix < 2; ix++)
					{
						OctreeNode< typename CubiquityVolumeType::VoxelType >* child = octreeNode->getChildNode(ix, iy, iz);
						if(child)
						{
							//Node* childNode = reinterpret_cast<Node*>(child->mGameEngineNode);
							GameplayOctreeNode* childGameplayOctreeNode = gameplayOctreeNode->mChildren[ix][iy][iz];

							if(childGameplayOctreeNode == 0)
							{
								childGameplayOctreeNode = new GameplayOctreeNode();
								childGameplayOctreeNode->mGameplayNode = gameplay::Node::create();

								Vector3I translation = child->mRegion.getLowerCorner() - octreeNode->mRegion.getLowerCorner();
								childGameplayOctreeNode->mGameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());

								gameplayOctreeNode->mChildren[ix][iy][iz] = childGameplayOctreeNode;

								gameplayOctreeNode->mGameplayNode->addChild(childGameplayOctreeNode->mGameplayNode);
							}

							syncNode(child, childGameplayOctreeNode);
						}
					}
				}
			}
		}

	protected:

		CubiquityVolumeType* mCubiquityVolume;
		GameplayOctreeNode* mRootGameplayOctreeNode;
	};
}

#include "GameplayVolume.inl"

#endif //GAMEPLAYVOLUME_H_