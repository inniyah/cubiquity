namespace Cubiquity
{
	template <typename CubiquityVolumeType>
	void GameplayVolume<CubiquityVolumeType>::initialiseOctree(void)
	{
		GP_ASSERT(mCubiquityVolume);
		GP_ASSERT(mCubiquityVolume->getRootOctreeNode());

		mRootGameplayOctreeNode = new GameplayOctreeNode();
		mRootGameplayOctreeNode->mGameplayNode = Node::create();

		std::stringstream ss;
		ss << "LOD = " << int(mCubiquityVolume->getRootOctreeNode()->mHeight) << ", Region = (" << mCubiquityVolume->getRootOctreeNode()->mRegion.getLowerX() << "," << mCubiquityVolume->getRootOctreeNode()->mRegion.getLowerY() << "," << mCubiquityVolume->getRootOctreeNode()->mRegion.getLowerZ() << ") to (" << mCubiquityVolume->getRootOctreeNode()->mRegion.getUpperX() << "," << mCubiquityVolume->getRootOctreeNode()->mRegion.getUpperY() << "," << mCubiquityVolume->getRootOctreeNode()->mRegion.getUpperZ() << ")";
		mRootGameplayOctreeNode->mGameplayNode->setId(ss.str().c_str());

		Vector3I translation = mCubiquityVolume->getRootOctreeNode()->mRegion.getLowerCorner();
		mRootGameplayOctreeNode->mGameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
	}

	template <typename CubiquityVolumeType>
	void GameplayVolume<CubiquityVolumeType>::syncNode(OctreeNode< typename CubiquityVolumeType::VoxelType >* octreeNode, GameplayOctreeNode* gameplayOctreeNode)
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
	}
}