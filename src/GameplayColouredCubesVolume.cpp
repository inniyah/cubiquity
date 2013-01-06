#include "GameplayColouredCubesVolume.h"

GameplayColouredCubesVolume::GameplayColouredCubesVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	:ColouredCubesVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth)
	,mRootGameplayNode(0)
{
	mRootGameplayNode = Node::create("RootGameplayNode");
}

GameplayColouredCubesVolume::~GameplayColouredCubesVolume()
{
}

void GameplayColouredCubesVolume::performUpdate(void)
{
	update();

	//Now ensure the gameplay node tree matches the one in the volume.

	if(mRootOctreeNode != 0)
	{
		syncNode(mRootOctreeNode, mRootGameplayNode);
	}
}

void GameplayColouredCubesVolume::syncNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode)
{
	octreeNode->mGameEngineNode = gameplayNode;
	gameplayNode->setUserPointer(octreeNode);

	if(octreeNode->parent)
	{
		Vector3DInt32 translation = octreeNode->mRegion.getLowerCorner() - octreeNode->parent->mRegion.getLowerCorner();
		gameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
	}

	if((octreeNode->mSmoothPolyVoxMesh) && (gameplayNode->getModel() == 0))
	{
		Model* model = octreeNode->buildModelFromPolyVoxMesh(octreeNode->mSmoothPolyVoxMesh);
		model->setMaterial("res/SmoothTerrain.material");
		gameplayNode->setModel(model);
		SAFE_RELEASE(model);

		PhysicsCollisionShape::Definition physDef = octreeNode->buildCollisionObjectFromPolyVoxMesh(octreeNode->mSmoothPolyVoxMesh);

		//Putting the physics mesh on LOD 0.
		PhysicsRigidBody::Parameters groundParams;
		groundParams.mass = 0.0f;
		gameplayNode->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, physDef, &groundParams);
	}

	if((octreeNode->mCubicPolyVoxMesh) && (gameplayNode->getModel() == 0))
	{
		Model* model = octreeNode->buildModelFromPolyVoxMesh(octreeNode->mCubicPolyVoxMesh);
		model->setMaterial("res/PolyVox.material");
		gameplayNode->setModel(model);
		SAFE_RELEASE(model);
	}

	for(int iz = 0; iz < 2; iz++)
	{
		for(int iy = 0; iy < 2; iy++)
		{
			for(int ix = 0; ix < 2; ix++)
			{
				if(octreeNode->children[ix][iy][iz] != 0)
				{
					Node* childNode = reinterpret_cast<Node*>(octreeNode->children[ix][iy][iz]->mGameEngineNode);
					if(childNode == 0)
					{
						childNode = Node::create("ChildGameplayNode");

						gameplayNode->addChild(childNode);
					}

					syncNode(octreeNode->children[ix][iy][iz], childNode);
				}
			}
		}
	}
}
