#include "GameplayColoredCubesVolume.h"

#include "Clock.h"
#include "ColoredCubicSurfaceExtractionTask.h"
#include "VolumeSerialisation.h"

#include "gameplay.h"

#include <stdio.h> //TEMPORARY!

using namespace gameplay;
using namespace PolyVox;

namespace Cubiquity
{
	GameplayColoredCubesVolume::GameplayColoredCubesVolume(const char* pathToVoxelDatabase, unsigned int baseNodeSize)
	{
		mCubiquityVolume = new ColoredCubesVolume(pathToVoxelDatabase, baseNodeSize);

		GP_ASSERT(mCubiquityVolume);
		GP_ASSERT(mCubiquityVolume->getRootOctreeNode());

		mRootGameplayOctreeNode = new GameplayOctreeNode< Color >(mCubiquityVolume->getRootOctreeNode(), 0);
	}

	GameplayColoredCubesVolume::~GameplayColoredCubesVolume()
	{
		delete mRootGameplayOctreeNode;
		delete mCubiquityVolume;
	}

	void GameplayColoredCubesVolume::performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold)
	{
		Vector3DFloat v3dViewPosition(viewPosition.x, viewPosition.y, viewPosition.z);
		mCubiquityVolume->update(v3dViewPosition, lodThreshold);

		//Now ensure the gameplay node tree matches the one in the volume.

		if(mCubiquityVolume->getRootOctreeNode() != 0)
		{
			syncNode(mCubiquityVolume->getRootOctreeNode(), mRootGameplayOctreeNode);
		}
	}

	gameplay::Model* GameplayColoredCubesVolume::buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< PositionMaterial<Color> >* polyVoxMesh)
	{
		//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
		const std::vector< PositionMaterial<Color> >& vecVertices = polyVoxMesh->getVertices();
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

			// Encode color in w component
			Color color = vecVertices[i].getMaterial();
			uint8_t red = color.getRed();
			uint8_t green = color.getGreen();
			uint8_t blue = color.getBlue();

			// A single precision float can eactly represent all integer values from 0 to 2^24 (http://www.mathworks.nl/help/matlab/ref/flintmax.html).
			// We can use therefore precisely and uniquely represent our three eight-bit colors but combining them into a single value as shown below.
			// In the shader we then extract the colors again. If we want to add alpha we will have to pass each component with only six bits of precision.
			float colorAsFloat = static_cast<float>(red * 65536 + green * 256 + blue);
			*ptr = colorAsFloat; ptr++;
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

	gameplay::Vector4 GameplayColoredCubesVolume::getVoxel(int x, int y, int z)
	{
		Color color = mCubiquityVolume->getVoxelAt(x, y, z);
		gameplay::Vector4 result(color.getRed() / 255.0f, color.getGreen() / 255.0f, color.getBlue() / 255.0f, color.getAlpha() / 255.0f);
		return result;
	}
	
	void GameplayColoredCubesVolume::setVoxel(int x, int y, int z, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, int updatePriority)
	{
		mCubiquityVolume->setVoxelAt(x, y, z, Color(red, green, blue, alpha), static_cast<UpdatePriority>(updatePriority));
	}

	void GameplayColoredCubesVolume::markAsModified(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, int updatePriority)
	{
		mCubiquityVolume->markAsModified(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), static_cast<UpdatePriority>(updatePriority));
	}

	void GameplayColoredCubesVolume::syncNode(OctreeNode< Color >* octreeNode, GameplayOctreeNode< Color >* gameplayOctreeNode)
	{
		if(gameplayOctreeNode->mMeshLastSyncronised < octreeNode->mMeshLastUpdated)
		{
			if(octreeNode->mPolyVoxMesh)
			{
				// Set up the renderable mesh
				Model* model = buildModelFromPolyVoxMesh(octreeNode->mPolyVoxMesh);
				model->setMaterial("res/Materials/ColoredCubicTerrain.material");

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
					OctreeNode< Color >* childOctreeNode = octreeNode->getChildNode(ix, iy, iz);
					if(childOctreeNode)
					{
						GameplayOctreeNode< Color >* childGameplayOctreeNode = gameplayOctreeNode->mChildren[ix][iy][iz];

						if(childGameplayOctreeNode == 0)
						{
							childGameplayOctreeNode = new GameplayOctreeNode< Color >(childOctreeNode, gameplayOctreeNode);
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

	gameplay::PhysicsCollisionShape::Definition GameplayColoredCubesVolume::buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterial<Color> >* polyVoxMesh)
	{
		//Now set up the physics
		const std::vector< ::PolyVox::PositionMaterial<Color> >& vecVertices = polyVoxMesh->getVertices();
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