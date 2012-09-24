#include "VolumeRegion.h"

#include "Volume.h"

#include "MeshPart.h"

#include <sstream>

using namespace gameplay;
using namespace PolyVox;

template <typename VoxelType>
VolumeRegion<VoxelType>::VolumeRegion(const Volume<VoxelType>* volume, PolyVox::Region region)
	:mRegion(region)
	,mVolume(volume)
{
	std::stringstream ss;
	ss << "VolumeRegionNode(" << mRegion.getLowerCorner().getX() << "," << mRegion.getLowerCorner().getY() << "," << mRegion.getLowerCorner().getZ() << ")";
	mNode = Node::create(ss.str().c_str());
	 //mNode->setTranslation(mRegion.getLowerCorner().getX(), mRegion.getLowerCorner().getY(), mRegion.getLowerCorner().getZ());
}

template <typename VoxelType>
VolumeRegion<VoxelType>::~VolumeRegion()
{
	 SAFE_RELEASE(mNode);
}

template <typename VoxelType>
void VolumeRegion<VoxelType>::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& polyVoxMesh)
{
	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<PositionMaterial>& vecVertices = polyVoxMesh.getVertices();
	const float* pVerticesConst = reinterpret_cast<const float*>(&vecVertices[0]);
	float* pVertices = const_cast<float*>(pVerticesConst);

	VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 4),
    };
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 1), polyVoxMesh.getVertices().size(), false);
    /*if (mesh == NULL)
    {
        return NULL;
    }*/
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    mesh->setVertexData(pVertices, 0, polyVoxMesh.getVertices().size());
	mesh->setBoundingBox(BoundingBox(Vector3(0,0,0), Vector3(16, 16, 16)));

	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<unsigned int>& vecIndices = polyVoxMesh.getIndices();
	const void* pIndicesConst = &vecIndices[0];
	void* pIndices = const_cast<void*>(pIndicesConst);
	MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX32, polyVoxMesh.getNoOfIndices());
	meshPart->setIndexData(pIndices, 0, vecIndices.size());

    Model* model = Model::create(mesh);
	switch(mVolume->getType())
	{
	case VolumeTypes::ColouredCubes:
		model->setMaterial("res/PolyVox.material");
		break;
	case VolumeTypes::SmoothTerrain:
		model->setMaterial("res/SmoothTerrain.material");
		break;
	default:
		//Add fallback material here
		break;
	}	
    SAFE_RELEASE(mesh);

	mNode->setModel(model);
}

template <typename VoxelType>
void VolumeRegion<VoxelType>::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyVoxMesh)
{
	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<PositionMaterialNormal>& vecVertices = polyVoxMesh.getVertices();
	const float* pVerticesConst = reinterpret_cast<const float*>(&vecVertices[0]);
	float* pVertices = const_cast<float*>(pVerticesConst);

	VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 4),
		VertexFormat::Element(VertexFormat::NORMAL, 3)
    };

	//Create the vertex data in the expected format
	float* vertexData = new float[polyVoxMesh.getVertices().size() * 7]; //7 float per vertex
	float* ptr = vertexData;
	for(int i = 0; i < vecVertices.size(); i++)
	{
		*ptr = vecVertices[i].getPosition().getX(); ptr++;
		*ptr = vecVertices[i].getPosition().getY(); ptr++;
		*ptr = vecVertices[i].getPosition().getZ(); ptr++;
		*ptr = 1.0;  ptr++;
		*ptr = vecVertices[i].getNormal().getX(); ptr++;
		*ptr = vecVertices[i].getNormal().getY(); ptr++;
		*ptr = vecVertices[i].getNormal().getZ(); ptr++;
	}

    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 2), polyVoxMesh.getVertices().size(), false);
    /*if (mesh == NULL)
    {
        return NULL;
    }*/
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    mesh->setVertexData(vertexData, 0, polyVoxMesh.getVertices().size());
	mesh->setBoundingBox(BoundingBox(Vector3(0,0,0), Vector3(16, 16, 16)));

	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<unsigned int>& vecIndices = polyVoxMesh.getIndices();
	const void* pIndicesConst = &vecIndices[0];
	void* pIndices = const_cast<void*>(pIndicesConst);
	MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX32, polyVoxMesh.getNoOfIndices());
	meshPart->setIndexData(pIndices, 0, vecIndices.size());

    Model* model = Model::create(mesh);
	switch(mVolume->getType())
	{
	case VolumeTypes::ColouredCubes:
		model->setMaterial("res/PolyVox.material");
		break;
	case VolumeTypes::SmoothTerrain:
		model->setMaterial("res/SmoothTerrain.material");
		break;
	default:
		//Add fallback material here
		break;
	}	
    SAFE_RELEASE(mesh);

	mNode->setModel(model);
}