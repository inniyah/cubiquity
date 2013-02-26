#include "GameplaySmoothTerrainVolume.h"

#include "Clock.h"

#include "gameplay.h"

using namespace gameplay;
using namespace PolyVox;

GameplaySmoothTerrainVolume::GameplaySmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:GameplayVolume<SmoothTerrainVolume>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize)
{
	mRootGameplayNode = createNodeWithExtraData< MultiMaterial >("RootGameplayNode");

	buildNode(mCubiquityVolume->mOctree->mRootOctreeNode, mRootGameplayNode);
}

GameplaySmoothTerrainVolume::~GameplaySmoothTerrainVolume()
{
}

void GameplaySmoothTerrainVolume::performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold)
{
	Vector3DFloat v3dViewPosition(viewPosition.x, viewPosition.y, viewPosition.z);
	mCubiquityVolume->update(v3dViewPosition, lodThreshold);

	//Now ensure the gameplay node tree matches the one in the volume.

	if(mCubiquityVolume->mOctree->mRootOctreeNode != 0)
	{
		syncNode(mCubiquityVolume->mOctree->mRootOctreeNode, mRootGameplayNode);
	}
}

void GameplaySmoothTerrainVolume::syncNode(OctreeNode< MultiMaterial >* octreeNode, gameplay::Node* gameplayNode)
{
	ExtraNodeData< MultiMaterial >* extraNodeData = static_cast<ExtraNodeData<  MultiMaterial >*>(gameplayNode->getUserPointer());
	extraNodeData->mOctreeNode = octreeNode;

	if(extraNodeData->mTimeStamp < octreeNode->mMeshLastUpdated)
	{
		if(octreeNode->mPolyVoxMesh)
		{
			Model* model = buildModelFromPolyVoxMesh(octreeNode->mPolyVoxMesh);
			model->setMaterial("res/SmoothTerrain.material");
			gameplayNode->setModel(model);
			SAFE_RELEASE(model);

			//There is a weird bug, whereby if we use the LOD 0 for physics it resets the node positions somehow. So we use LOD 1 here.
			if(octreeNode->mLodLevel == 1)
			{
				PhysicsCollisionShape::Definition physDef = buildCollisionObjectFromPolyVoxMesh(octreeNode->mPolyVoxMesh);

				PhysicsRigidBody::Parameters groundParams;
				groundParams.mass = 0.0f;
				gameplayNode->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, physDef, &groundParams);
			}
		}	

		extraNodeData->mTimeStamp = Clock::getTimestamp();
	}

	if(octreeNode->mRenderThisNode)
	{
		gameplayNode->setTag("RenderThisNode", "t");
	}
	else
	{
		gameplayNode->setTag("RenderThisNode", "f");
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
					GP_ASSERT(childNode);

					syncNode(octreeNode->children[ix][iy][iz], childNode);
				}
			}
		}
	}
}

gameplay::Model* GameplaySmoothTerrainVolume::buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal<MultiMaterialMarchingCubesController::MaterialType > >* polyVoxMesh)
{
	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<PositionMaterialNormal<MultiMaterialMarchingCubesController::MaterialType> >& vecVertices = polyVoxMesh->getVertices();
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

		// Material values range from 0 - getMaxMaterialValue() for each voxel. At the position
		// of the isosurface materials are not at their full intensity (they are at roughly half
		// because that's where the theshold is). We need to normalise the values to thier full range.
		Vector<4, float> matAsVec = vecVertices[i].getMaterial();
		matAsVec.normalise();

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

PhysicsCollisionShape::Definition GameplaySmoothTerrainVolume::buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal<MultiMaterialMarchingCubesController::MaterialType > >* polyVoxMesh)
{
	//Now set up the physics
	const std::vector<PositionMaterialNormal<MultiMaterialMarchingCubesController::MaterialType> >& vecVertices = polyVoxMesh->getVertices();
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
