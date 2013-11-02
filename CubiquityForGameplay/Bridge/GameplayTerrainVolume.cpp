#include "GameplayTerrainVolume.h"

#include "Clock.h"
#include "TerrainVolumeGenerator.h"
#include "VolumeSerialisation.h"

#include "gameplay.h" //Would rather have this last, but it's being upset by Timer.h including Windows.h

using namespace gameplay;
using namespace PolyVox;

namespace Cubiquity
{

	GameplayTerrainVolume::GameplayTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, const char* pageFolder, unsigned int baseNodeSize, bool createFloor, unsigned int floorDepth)
	{
		mCubiquityVolume = createTerrainVolume(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), pageFolder, baseNodeSize, createFloor, floorDepth);

		if(createFloor)
		{
			generateFloor(mCubiquityVolume, floorDepth-2, 0, floorDepth, 1);
		}

		mRootGameplayOctreeNode = new GameplayOctreeNode< MaterialSet >(mCubiquityVolume->getRootOctreeNode(), 0);
	}

	GameplayTerrainVolume::GameplayTerrainVolume(const char* dataToLoad, const char* pageFolder, unsigned int baseNodeSize)
	{
		// Check whether the provided data is a file or a directory
		FILE* file = fopen(dataToLoad, "rb");
		if(file)
		{
			// For now we assume it's .vxl
			//mCubiquityVolume = importVxl(dataToLoad);
			POLYVOX_ASSERT(false, ".vxl data cannot be loaded into smooth terrain volumes");
		}
		else
		{
			// For now we assume it's VolDat. Leter on we should check for
			// Volume.idx and load raw Cubiquity data instead if necessary.
			mCubiquityVolume = importVolDat<TerrainVolumeImpl>(dataToLoad, pageFolder, baseNodeSize);
		}

		mRootGameplayOctreeNode = new GameplayOctreeNode< MaterialSet >(mCubiquityVolume->getRootOctreeNode(), 0);
	}

	GameplayTerrainVolume::~GameplayTerrainVolume()
	{
		delete mRootGameplayOctreeNode;
		delete mCubiquityVolume;
	}

	void GameplayTerrainVolume::performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold)
	{
		Vector3DFloat v3dViewPosition(viewPosition.x, viewPosition.y, viewPosition.z);
		mCubiquityVolume->update(v3dViewPosition, lodThreshold);

		//Now ensure the gameplay node tree matches the one in the volume.

		if(mCubiquityVolume->getRootOctreeNode() != 0)
		{
			syncNode(mCubiquityVolume->getRootOctreeNode(), mRootGameplayOctreeNode);
		}
	}

	gameplay::Model* GameplayTerrainVolume::buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterialNormal< MaterialSet > >* polyVoxMesh)
	{
		//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
		const std::vector< ::PolyVox::PositionMaterialNormal< MaterialSet > >& vecVertices = polyVoxMesh->getVertices();
		const float* pVerticesConst = reinterpret_cast<const float*>(&vecVertices[0]);
		float* pVertices = const_cast<float*>(pVerticesConst);

		VertexFormat::Element elements[] =
		{
			VertexFormat::Element(VertexFormat::POSITION, 4),
			VertexFormat::Element(VertexFormat::TEXCOORD0, 4),
			VertexFormat::Element(VertexFormat::NORMAL, 3)
		};

		//Create the vertex data in the expected format
		float* vertexData = new float[polyVoxMesh->getVertices().size() * 11]; //11 float per vertex
		float* ptr = vertexData;
		for(uint32_t i = 0; i < vecVertices.size(); i++)
		{
			*ptr = vecVertices[i].getPosition().getX(); ptr++;
			*ptr = vecVertices[i].getPosition().getY(); ptr++;
			*ptr = vecVertices[i].getPosition().getZ(); ptr++;
			*ptr = 1.0;  ptr++;

			// Material values range from 0 - getMaxMaterialValue() for each voxel. At the position of the isosurface materials are not at their
			// full intensity (they are at roughly half because that's where the theshold is). In theory the components should sum to 128 and we
			// should be able to divide by that to get material components summing to 1.0, but in practice we have found this doesn't quite work.
			// Full normalization seems to be required (and maybe this should be done inside Cubiquity) but we can investigate more in the future.
			Vector<8, float> matAsVec = vecVertices[i].getMaterial();
			//matAsVec /= 128.0f;
			float sum = matAsVec.getElement(0) + matAsVec.getElement(1) + matAsVec.getElement(2) + matAsVec.getElement(3);
			if(sum > 0.001) // Avoid divide by zero
			{
				matAsVec /= sum;
			}

			*ptr = matAsVec.getElement(0); ptr++;
			*ptr = matAsVec.getElement(1); ptr++;
			*ptr = matAsVec.getElement(2); ptr++;
			*ptr = matAsVec.getElement(3); ptr++;
		
			*ptr = vecVertices[i].getNormal().getX(); ptr++;
			*ptr = vecVertices[i].getNormal().getY(); ptr++;
			*ptr = vecVertices[i].getNormal().getZ(); ptr++;
		}

		Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), polyVoxMesh->getVertices().size(), false);
		/*if (mesh == NULL)
		{
			return NULL;
		}*/
		mesh->setPrimitiveType(Mesh::TRIANGLES);
		mesh->setVertexData(vertexData, 0, polyVoxMesh->getVertices().size());
		mesh->setBoundingBox(BoundingBox(Vector3(0,0,0), Vector3(16, 16, 16)));
		delete[] vertexData;

		//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
		const std::vector<unsigned int>& vecIndices = polyVoxMesh->getIndices();
		const void* pIndicesConst = &vecIndices[0];
		void* pIndices = const_cast<void*>(pIndicesConst);
		MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX32, polyVoxMesh->getNoOfIndices());
		meshPart->setIndexData(pIndices, 0, vecIndices.size());

		Model* model = Model::create(mesh);
		SAFE_RELEASE(mesh);

		return model;
	}

	void GameplayTerrainVolume::syncNode(OctreeNode< MaterialSet >* octreeNode, GameplayOctreeNode< MaterialSet >* gameplayOctreeNode)
	{
		if(gameplayOctreeNode->mMeshLastSyncronised < octreeNode->mMeshLastUpdated)
		{
			if(octreeNode->mPolyVoxMesh)
			{
				// Set up the renderable mesh
				Model* model = buildModelFromPolyVoxMesh(octreeNode->mPolyVoxMesh);
				model->setMaterial("res/Materials/Terrain.material");

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
					OctreeNode< MaterialSet >* childOctreeNode = octreeNode->getChildNode(ix, iy, iz);
					if(childOctreeNode)
					{
						GameplayOctreeNode< MaterialSet >* childGameplayOctreeNode = gameplayOctreeNode->mChildren[ix][iy][iz];

						if(childGameplayOctreeNode == 0)
						{
							childGameplayOctreeNode = new GameplayOctreeNode< MaterialSet >(childOctreeNode, gameplayOctreeNode);
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

	gameplay::PhysicsCollisionShape::Definition GameplayTerrainVolume::buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterialNormal< MaterialSet > >* polyVoxMesh)
	{
		//Now set up the physics
		const std::vector< ::PolyVox::PositionMaterialNormal< MaterialSet > >& vecVertices = polyVoxMesh->getVertices();
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
