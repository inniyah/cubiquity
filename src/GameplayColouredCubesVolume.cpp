#include "GameplayColouredCubesVolume.h"

#include "ExtraNodeData.h"

GameplayColouredCubesVolume::GameplayColouredCubesVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	:ColouredCubesVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth)
	,mRootGameplayNode(0)
{
	mRootGameplayNode = createNodeWithExtraData("RootGameplayNode");
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
	ExtraNodeData* extraNodeData = static_cast<ExtraNodeData*>(gameplayNode->getUserPointer());
	extraNodeData->mOctreeNode = octreeNode;

	if(octreeNode->parent)
	{
		Vector3DInt32 translation = octreeNode->mRegion.getLowerCorner() - octreeNode->parent->mRegion.getLowerCorner();
		gameplayNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
	}

	if(extraNodeData->mTimeStamp < octreeNode->mDataLastModified)
	{
		if(octreeNode->mCubicPolyVoxMesh)
		{
			Model* model = buildModelFromPolyVoxMesh(octreeNode->mCubicPolyVoxMesh);
			model->setMaterial("res/PolyVox.material");
			gameplayNode->setModel(model);
			SAFE_RELEASE(model);
		}

		extraNodeData->mTimeStamp = getTime();
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
						childNode = createNodeWithExtraData("ChildGameplayNode");

						gameplayNode->addChild(childNode);
					}

					syncNode(octreeNode->children[ix][iy][iz], childNode);
				}
			}
		}
	}
}

gameplay::Model* GameplayColouredCubesVolume::buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterial<Colour> >* polyVoxMesh)
{
	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<PositionMaterial<Colour> >& vecVertices = polyVoxMesh->getVertices();
	const float* pVerticesConst = reinterpret_cast<const float*>(&vecVertices[0]);
	float* pVertices = const_cast<float*>(pVerticesConst);

	VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 4),
    };

	//Create the vertex data in the expected format
	float* vertexData = new float[polyVoxMesh->getVertices().size() * 4]; //4 float per vertex
	float* ptr = vertexData;
	for(int i = 0; i < vecVertices.size(); i++)
	{
		// Position stored in x,y,z components.
		*ptr = vecVertices[i].getPosition().getX(); ptr++;
		*ptr = vecVertices[i].getPosition().getY(); ptr++;
		*ptr = vecVertices[i].getPosition().getZ(); ptr++;

		// Encode colour in w component
		Colour colour = vecVertices[i].getMaterial();
		uint16_t colourAsUint = (colour.getRed() << 12) | (colour.getGreen() << 8) | (colour.getBlue() << 4) | (colour.getAlpha());
		*ptr = static_cast<float>(colourAsUint); ptr++;
	}

    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 1), polyVoxMesh->getVertices().size(), false);
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