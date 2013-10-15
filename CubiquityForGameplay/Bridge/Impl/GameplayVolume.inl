namespace Cubiquity
{
	/*template <typename CubiquityVolumeType>
	void GameplayVolume<CubiquityVolumeType>::initialiseOctree(void)
	{
		GP_ASSERT(mCubiquityVolume);
		GP_ASSERT(mCubiquityVolume->getRootOctreeNode());

		mRootGameplayOctreeNode = new GameplayOctreeNode< typename CubiquityVolumeType::VoxelType >(mCubiquityVolume->getRootOctreeNode(), 0);

		
	}*/

	/*template <typename CubiquityVolumeType>
	void GameplayVolume<CubiquityVolumeType>::syncNode(OctreeNode< typename CubiquityVolumeType::VoxelType >* octreeNode, GameplayOctreeNode< typename CubiquityVolumeType::VoxelType >* gameplayOctreeNode)
	{
		if(gameplayOctreeNode->mMeshLastSyncronised < octreeNode->mMeshLastUpdated)
		{
			if(octreeNode->mPolyVoxMesh)
			{
				// Set up the renderable mesh
				Model* model = buildModelFromPolyVoxMesh(octreeNode->mPolyVoxMesh);
				if(VoxelTraits<typename CubiquityVolumeType::VoxelType>::IsColour)
				{
					model->setMaterial("res/Materials/ColouredCubicTerrain.material");
				}
				else
				{
					model->setMaterial("res/Materials/Terrain.material");
				}
				gameplayOctreeNode->mGameplayNode->setModel(model);
				SAFE_RELEASE(model);

				// Set up the collision mesh
				PhysicsCollisionShape::Definition physDef = buildCollisionObjectFromPolyVoxMesh(octreeNode->mPolyVoxMesh);
				PhysicsRigidBody::Parameters groundParams;
				groundParams.mass = 0.0f;

				// From docs: A kinematic collision object is an object that is not simulated by the physics system and instead has its transform driven manually.
				// I'm not exactly clear how this differs from static, but this kinematic flag is used in Node::getWorldMatrix() to decide whether to use hierarchy.
				groundParams.kinematic = true;
				gameplayOctreeNode->mGameplayNode->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, physDef, &groundParams);
			}	
			else
			{
				gameplayOctreeNode->mGameplayNode->setModel(0);
				gameplayOctreeNode->mGameplayNode->setCollisionObject(PhysicsCollisionObject::NONE);
			}

			gameplayOctreeNode->mMeshLastSyncronised = Clock::getTimestamp();
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
					OctreeNode< typename CubiquityVolumeType::VoxelType >* childOctreeNode = octreeNode->getChildNode(ix, iy, iz);
					if(childOctreeNode)
					{
						GameplayOctreeNode< typename CubiquityVolumeType::VoxelType >* childGameplayOctreeNode = gameplayOctreeNode->mChildren[ix][iy][iz];

						if(childGameplayOctreeNode == 0)
						{
							childGameplayOctreeNode = new GameplayOctreeNode< typename CubiquityVolumeType::VoxelType >(childOctreeNode, gameplayOctreeNode);
							gameplayOctreeNode->mChildren[ix][iy][iz] = childGameplayOctreeNode;							
						}

						syncNode(childOctreeNode, childGameplayOctreeNode);
					}
					else
					{
						// If the child doesn't exist in the cubiquity  octree then make sure it doesn't exist in the gameplay octree.
						if(gameplayOctreeNode->mChildren[ix][iy][iz])
						{
							delete gameplayOctreeNode->mChildren[ix][iy][iz];
							gameplayOctreeNode->mChildren[ix][iy][iz] = 0;
						}
					}
				}
			}
		}
	}

	template <typename CubiquityVolumeType>
	gameplay::PhysicsCollisionShape::Definition GameplayVolume<CubiquityVolumeType>::buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< typename VoxelTraits<typename CubiquityVolumeType::VoxelType>::VertexType>* polyVoxMesh)
	{
		//Now set up the physics
		const std::vector< typename VoxelTraits<typename CubiquityVolumeType::VoxelType>::VertexType >& vecVertices = polyVoxMesh->getVertices();
		const std::vector<unsigned int>& vecIndices = polyVoxMesh->getIndices();
		float* vertexData = new float[polyVoxMesh->getVertices().size() * 3];

		unsigned int* physicsIndices = new unsigned int [vecIndices.size()];
		for(uint32_t ct = 0; ct < vecIndices.size(); ct++)
		{
			physicsIndices[ct] = vecIndices[ct];
		}

		float* ptr = vertexData;
		for(uint32_t i = 0; i < vecVertices.size(); i++)
		{
			// Position stored in x,y,z components.
			*ptr = vecVertices[i].getPosition().getX(); ptr++;
			*ptr = vecVertices[i].getPosition().getY(); ptr++;
			*ptr = vecVertices[i].getPosition().getZ(); ptr++;
		}

		return PhysicsCollisionShape::custom(vertexData, polyVoxMesh->getVertices().size(), physicsIndices, vecIndices.size());
	}*/
}