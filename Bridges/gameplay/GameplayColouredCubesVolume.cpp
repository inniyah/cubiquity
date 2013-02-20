#include "GameplayColouredCubesVolume.h"

#include "gameplay.h"

#include <stdio.h> //TEMPORARY!

using namespace gameplay;
using namespace PolyVox;

GameplayColouredCubesVolume::GameplayColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:GameplayVolume<ColouredCubesVolume>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize)
	,mColouredCubicSurfaceExtractionTaskProcessor(0)
{
	mColouredCubicSurfaceExtractionTaskProcessor = new TaskProcessor<ColouredCubicSurfaceExtractionTask>;
	mCubiquityVolume->mColouredCubicSurfaceExtractionTaskProcessor = mColouredCubicSurfaceExtractionTaskProcessor;

	mRootGameplayNode = createNodeWithExtraData("RootGameplayNode");

	buildNode(mCubiquityVolume->mRootOctreeNode, mRootGameplayNode);
}

GameplayColouredCubesVolume::~GameplayColouredCubesVolume()
{
	delete mColouredCubicSurfaceExtractionTaskProcessor;
}

void GameplayColouredCubesVolume::performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold)
{
	Vector3DFloat v3dViewPosition(viewPosition.x, viewPosition.y, viewPosition.z);
	mCubiquityVolume->update(v3dViewPosition, lodThreshold);

	//Now ensure the gameplay node tree matches the one in the volume.

	if(mCubiquityVolume->mRootOctreeNode != 0)
	{
		syncNode(mCubiquityVolume->mRootOctreeNode, mRootGameplayNode);
	}
}

void GameplayColouredCubesVolume::syncNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode)
{
	ExtraNodeData* extraNodeData = static_cast<ExtraNodeData*>(gameplayNode->getUserPointer());
	extraNodeData->mOctreeNode = octreeNode;

	if(extraNodeData->mTimeStamp < octreeNode->mMeshLastUpdated)
	{
		if(octreeNode->mCubicPolyVoxMesh)
		{
			Model* model = buildModelFromPolyVoxMesh(octreeNode->mCubicPolyVoxMesh);
			model->setMaterial("res/PolyVox.material");
			gameplayNode->setModel(model);
			SAFE_RELEASE(model);
		}

		extraNodeData->mTimeStamp = mCubiquityVolume->getTime();
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

gameplay::Vector4 GameplayColouredCubesVolume::getVoxel(int x, int y, int z)
{
	Colour colour = mCubiquityVolume->getVoxelAt(x, y, z);
	gameplay::Vector4 result(colour.getRedAsFloat(), colour.getGreenAsFloat(), colour.getBlueAsFloat(), colour.getAlphaAsFloat());
	return result;
}

void GameplayColouredCubesVolume::setVoxel(int x, int y, int z, const gameplay::Vector4& colour, bool markAsModified)
{
	mCubiquityVolume->setVoxelAt(x, y, z, Colour(colour.x, colour.y, colour.z, colour.w), markAsModified);
}

void GameplayColouredCubesVolume::setVoxel(int x, int y, int z, float red, float green, float blue, float alpha, bool markAsModified)
{
	mCubiquityVolume->setVoxelAt(x, y, z, Colour(red, green, blue, alpha), markAsModified);
}

void GameplayColouredCubesVolume::markRegionAsModified(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ)
{
	mCubiquityVolume->markRegionAsModified(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ));
}

GameplayColouredCubesVolume* GameplayColouredCubesVolume::importVxl(const char* filename)
{
	GameplayColouredCubesVolume* result = create(0, 0, 0, 511, 63, 511, 64, 64);
	/*for(int z = 0; z < 64; z++)
	{
		for(int y = 0; y < 128; y++)
		{
			for(int x = 0; x < 128; x++)
			{
				result->setVoxel(x, y, z, Vector4(1.0, 0.0, 0.0, 1.0), false);
			}
		}
	}*/

	FILE* inputFile = fopen("VxlEditor\\ToLoad.vxl", "rb");
	POLYVOX_ASSERT(inputFile, "Failed to open input file!");

	// Determine input file's size.
	fseek(inputFile, 0, SEEK_END);
	long fileSize = ftell(inputFile);
	fseek(inputFile, 0, SEEK_SET);

	uint8_t* data = new uint8_t[fileSize];
	long bytesRead = fread(data, sizeof(uint8_t), fileSize, inputFile);
	POLYVOX_ASSERT(fileSize == bytesRead, "Failed to read file!");

	uint8_t N, S, E, A, K, Z, M, colorI, zz, runlength, j, red, green, blue;

	int p;

	int i = 0;
	int x = 0;
	int y = 0;
	int columnI = 0;
	int mapSize = 512;
	int columnCount = mapSize * mapSize;
	while (columnI < columnCount)
	{
		// i = span start byte
		N = data[i];
		S = data[i + 1];
		E = data[i + 2];
		A = data[i + 3];
		K = E - S + 1;
		if (N == 0)
		{
			Z = 0;
			M = 64;
		} else
		{
			Z = (N-1) - K;
			// A of the next span
			M = data[i + N * 4 + 3];
		}
		colorI = 0;
		for (p = 0; p < 2; p++)
		{
			// BEWARE: COORDINATE SYSTEM TRANSFORMATIONS MAY BE NEEDED
			// Get top run of colors
			if (p == 0)
			{
				zz = S;
				runlength = K;
			} else
			{
				// Get bottom run of colors
				zz = M - Z;
				runlength = Z;
			}
			for (j = 0; j < runlength; j++)
			{
				red = data[i + 6 + colorI * 4];
				green = data[i + 5 + colorI * 4];
				blue = data[i + 4 + colorI * 4];
				// Do something with these colors
				//makeVoxelColorful(x, y, zz, red, green, blue);
				result->setVoxel(x, zz, y, Vector4(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0), false);
				zz++;
				colorI++;
			}
		}
		// Now deal with solid non-surface voxels
		// No color data is provided for non-surface voxels
		zz = E + 1;
		runlength = M - Z - zz;
		for (j = 0; j < runlength; j++)
		{
			//makeVoxelSolid(x, y, zz);
			result->setVoxel(x, zz, y, Vector4(1.0, 0.0, 1.0, 1.0), false);
			zz++;
		}
		if (N == 0)
		{
			columnI++;
			x++;
			if (x >= mapSize)
			{
				x = 0;
				y++;
			}
			i += 4*(1 + K);
		}
		else
		{
			i += N * 4;
		}
	}






	result->markRegionAsModified(0, 0, 0, 127, 127, 63);

	return result;
}