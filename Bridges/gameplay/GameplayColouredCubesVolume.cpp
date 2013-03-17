#include "GameplayColouredCubesVolume.h"

#include "Clock.h"
#include "ColouredCubicSurfaceExtractionTask.h"

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

		buildNode(mCubiquityVolume->mOctree->mRootOctreeNode, mRootGameplayNode);
	}

	GameplayColouredCubesVolume::GameplayColouredCubesVolume(ColouredCubesVolume* colouredCubesVolume)
		:GameplayVolume<ColouredCubesVolume>(colouredCubesVolume)
	{
		mRootGameplayNode = createNodeWithExtraData< Colour >("RootGameplayNode");

		buildNode(mCubiquityVolume->mOctree->mRootOctreeNode, mRootGameplayNode);
	}

	GameplayColouredCubesVolume::~GameplayColouredCubesVolume()
	{
	}

	void GameplayColouredCubesVolume::performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold)
	{
		Vector3DFloat v3dViewPosition(viewPosition.x, viewPosition.y, viewPosition.z);
		mCubiquityVolume->update(v3dViewPosition, lodThreshold);

		//Now ensure the gameplay node tree matches the one in the volume.

		if(mCubiquityVolume->mOctree->mRootOctreeNode != 0)
		{
			syncNode(mCubiquityVolume->mOctree->mRootOctreeNode, mRootGameplayNode);
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

	void GameplayColouredCubesVolume::setVoxel(int x, int y, int z, const gameplay::Vector4& colour, int updatePriority)
	{
		mCubiquityVolume->setVoxelAt(x, y, z, Colour(colour.x, colour.y, colour.z, colour.w), static_cast<UpdatePriority>(updatePriority));
	}

	void GameplayColouredCubesVolume::setVoxel(int x, int y, int z, float red, float green, float blue, float alpha, int updatePriority)
	{
		mCubiquityVolume->setVoxelAt(x, y, z, Colour(red, green, blue, alpha), static_cast<UpdatePriority>(updatePriority));
	}

	void GameplayColouredCubesVolume::markAsModified(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, int updatePriority)
	{
		mCubiquityVolume->markAsModified(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), static_cast<UpdatePriority>(updatePriority));
	}

	GameplayColouredCubesVolume* GameplayColouredCubesVolume::importVxl(const char* filename)
	{
		GameplayColouredCubesVolume* result = create(0, 0, 0, 511, 63, 511, 64, 64);

		FILE* inputFile = fopen(filename, "rb");
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
					result->setVoxel(x, 63 - zz, y, Vector4(red / 255.0f, green / 255.0f, blue / 255.0f, 1.0), UpdatePriorities::DontUpdate);
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
				result->setVoxel(x, 63 - zz, y, Vector4(1.0, 0.0, 1.0, 1.0), UpdatePriorities::DontUpdate);
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






		result->markAsModified(0, 0, 0, 511, 63, 511, UpdatePriorities::Background);

		return result;
	}
}