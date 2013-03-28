#include "GameplayColouredCubesVolume.h"

#include "Clock.h"
#include "ColouredCubicSurfaceExtractionTask.h"
#include "VolumeSerialisation.h"

#include "gameplay.h"

#include <stdio.h> //TEMPORARY!

using namespace gameplay;
using namespace PolyVox;

namespace Cubiquity
{
	GameplayColouredCubesVolume::GameplayColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
		:GameplayVolume<ColouredCubesVolume>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize)
	{
		mRootGameplayNode = createNodeWithExtraData< Colour >("RootGameplayNode");

		buildNode(mCubiquityVolume->getRootOctreeNode(), mRootGameplayNode);
	}

	GameplayColouredCubesVolume::GameplayColouredCubesVolume(const char* dataToLoad)
	{
		// Check whether the provided data is a file or a directory
		FILE* file = fopen(dataToLoad, "rb");
		if(file)
		{
			// For now we assume it's .vxl
			mCubiquityVolume = importVxl(dataToLoad);
		}
		else
		{
			// For now we assume it's VolDat. Leter on we should check for
			// Volume.idx and load raw Cubiquity data instead if necessary.
			mCubiquityVolume = importVolDat<ColouredCubesVolume>(dataToLoad);
		}

		mRootGameplayNode = createNodeWithExtraData< Colour >("RootGameplayNode");

		buildNode(mCubiquityVolume->getRootOctreeNode(), mRootGameplayNode);
	}

	GameplayColouredCubesVolume::GameplayColouredCubesVolume(ColouredCubesVolume* colouredCubesVolume)
		:GameplayVolume<ColouredCubesVolume>(colouredCubesVolume)
	{
		mRootGameplayNode = createNodeWithExtraData< Colour >("RootGameplayNode");

		buildNode(mCubiquityVolume->getRootOctreeNode(), mRootGameplayNode);
	}

	GameplayColouredCubesVolume::~GameplayColouredCubesVolume()
	{
	}

	void GameplayColouredCubesVolume::performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold)
	{
		Vector3DFloat v3dViewPosition(viewPosition.x, viewPosition.y, viewPosition.z);
		mCubiquityVolume->update(v3dViewPosition, lodThreshold);

		//Now ensure the gameplay node tree matches the one in the volume.

		if(mCubiquityVolume->getRootOctreeNode() != 0)
		{
			syncNode(mCubiquityVolume->getRootOctreeNode(), mRootGameplayNode);
		}
	}

	void GameplayColouredCubesVolume::syncNode(OctreeNode< Colour >* octreeNode, gameplay::Node* gameplayNode)
	{
		ExtraNodeData< Colour >* extraNodeData = static_cast<ExtraNodeData< Colour >*>(gameplayNode->getUserPointer());
		extraNodeData->mOctreeNode = octreeNode;

		if(extraNodeData->mTimeStamp < octreeNode->mMeshLastUpdated)
		{
			if(octreeNode->mPolyVoxMesh)
			{
				Model* model = buildModelFromPolyVoxMesh(octreeNode->mPolyVoxMesh);
				model->setMaterial("res/PolyVox.material");
				gameplayNode->setModel(model);
				SAFE_RELEASE(model);
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

	gameplay::Model* GameplayColouredCubesVolume::buildModelFromPolyVoxMesh(const ::PolyVox::SurfaceMesh< ::PolyVox::PositionMaterial<Colour> >* polyVoxMesh)
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
		for(uint32_t i = 0; i < vecVertices.size(); i++)
		{
			// Position stored in x,y,z components.
			*ptr = vecVertices[i].getPosition().getX(); ptr++;
			*ptr = vecVertices[i].getPosition().getY(); ptr++;
			*ptr = vecVertices[i].getPosition().getZ(); ptr++;

			// Encode colour in w component
			Colour colour = vecVertices[i].getMaterial();
			uint8_t red = colour.getRed();
			uint8_t green = colour.getGreen();
			uint8_t blue = colour.getBlue();

			// A single precision float can eactly represent all integer values from 0 to 2^24 (http://www.mathworks.nl/help/matlab/ref/flintmax.html).
			// We can use therefore precisely and uniquely represent our three eight-bit colours but combining them into a single value as shown below.
			// In the shader we then extract the colours again. If we want to add alpha we will have to pass each component with only six bits of precision.
			float colourAsFloat = static_cast<float>(red * 65536 + green * 256 + blue);
			*ptr = colourAsFloat; ptr++;
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
		gameplay::Vector4 result(colour.getRed() / 255.0f, colour.getGreen() / 255.0f, colour.getBlue() / 255.0f, colour.getAlpha() / 255.0f);
		return result;
	}
	
	void GameplayColouredCubesVolume::setVoxel(int x, int y, int z, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, int updatePriority)
	{
		mCubiquityVolume->setVoxelAt(x, y, z, Colour(red, green, blue, alpha), static_cast<UpdatePriority>(updatePriority));
	}

	void GameplayColouredCubesVolume::markAsModified(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, int updatePriority)
	{
		mCubiquityVolume->markAsModified(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), static_cast<UpdatePriority>(updatePriority));
	}
}