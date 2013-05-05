namespace Cubiquity
{
	template <typename VolumeType>
	void GameplayVolume<VolumeType>::initialiseOctree(void)
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

	template <typename _VolumeType>
	gameplay::PhysicsCollisionShape::Definition GameplayVolume<_VolumeType>::buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< typename VoxelTraits<typename _VolumeType::VoxelType>::VertexType>* polyVoxMesh)
	{
		//Now set up the physics
		const std::vector< typename VoxelTraits<typename VolumeType::VoxelType>::VertexType >& vecVertices = polyVoxMesh->getVertices();
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