#include "GameplaySmoothTerrainVolume.h"

#include "Clock.h"
#include "VolumeSerialisation.h"

#include "gameplay.h"

using namespace gameplay;
using namespace PolyVox;

namespace Cubiquity
{

	GameplaySmoothTerrainVolume::GameplaySmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int baseNodeSize, const char* pageFolder)
		:GameplayVolume<SmoothTerrainVolume>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, baseNodeSize, pageFolder)
	{
		initialiseOctree();
	}

	GameplaySmoothTerrainVolume::GameplaySmoothTerrainVolume(const char* dataToLoad, unsigned int baseNodeSize, const char* pageFolder)
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
			mCubiquityVolume = importVolDat<SmoothTerrainVolume>(dataToLoad, baseNodeSize, pageFolder);
		}

		initialiseOctree();
	}

	GameplaySmoothTerrainVolume::~GameplaySmoothTerrainVolume()
	{
	}

	void GameplaySmoothTerrainVolume::performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold)
	{
		Vector3DFloat v3dViewPosition(viewPosition.x, viewPosition.y, viewPosition.z);
		mCubiquityVolume->update(v3dViewPosition, lodThreshold);

		//Now ensure the gameplay node tree matches the one in the volume.

		if(mCubiquityVolume->getRootOctreeNode() != 0)
		{
			syncNode(mCubiquityVolume->getRootOctreeNode(), mRootGameplayOctreeNode);
		}
	}

	gameplay::Model* GameplaySmoothTerrainVolume::buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< typename VoxelTraits<typename CubiquityVolumeType::VoxelType>::VertexType>* polyVoxMesh)
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
			if(matAsVec.lengthSquared() > 0.001f)
			{
				matAsVec.normalise();
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
}
