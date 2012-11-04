#include "VolumeRegion.h"

#include "Volume.h"

#include "MeshPart.h"

#include <sstream>

using namespace gameplay;
using namespace PolyVox;

void lodCleanupCallback(void* ptr)
{
	uint32_t* pLodLevel = static_cast<uint32_t*>(ptr);
	if(pLodLevel)
	{
		delete pLodLevel;
	}
}

VolumeRegion::VolumeRegion(PolyVox::Region region, Node* parentNode)
	:mRegion(region)
	,mIsMeshUpToDate(false)
{
	for(uint32_t lod = 0; lod < NoOfLodLevels; lod++)
	{
		std::stringstream ss;
		ss << "VolumeRegionNode(" << mRegion.getLowerCorner().getX() << "," << mRegion.getLowerCorner().getY() << "," << mRegion.getLowerCorner().getZ() << "), LOD = " << lod;
		mNode[lod] = Node::create(ss.str().c_str());
		parentNode->addChild(mNode[lod]);
		 mNode[lod]->setTranslation(mRegion.getLowerCorner().getX(), mRegion.getLowerCorner().getY(), mRegion.getLowerCorner().getZ());
	}
}

VolumeRegion::~VolumeRegion()
{
	for(uint32_t lod = 0; lod < NoOfLodLevels; lod++)
	{
		SAFE_RELEASE(mNode[lod]);
	}
}

void VolumeRegion::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<Colour> >& polyVoxMesh, uint32_t lod)
{
	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<PositionMaterial<Colour> >& vecVertices = polyVoxMesh.getVertices();
	const float* pVerticesConst = reinterpret_cast<const float*>(&vecVertices[0]);
	float* pVertices = const_cast<float*>(pVerticesConst);

	VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 4),
    };

	//Create the vertex data in the expected format
	float* vertexData = new float[polyVoxMesh.getVertices().size() * 4]; //4 float per vertex
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

    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 1), polyVoxMesh.getVertices().size(), false);
    /*if (mesh == NULL)
    {
        return NULL;
    }*/
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    mesh->setVertexData(vertexData, 0, polyVoxMesh.getVertices().size());
	mesh->setBoundingBox(BoundingBox(Vector3(0,0,0), Vector3(16, 16, 16)));
	delete[] vertexData;

	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<unsigned int>& vecIndices = polyVoxMesh.getIndices();
	const void* pIndicesConst = &vecIndices[0];
	void* pIndices = const_cast<void*>(pIndicesConst);
	MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX32, polyVoxMesh.getNoOfIndices());
	meshPart->setIndexData(pIndices, 0, vecIndices.size());

    Model* model = Model::create(mesh);
    SAFE_RELEASE(mesh);

	mNode[lod]->setModel(model);
	SAFE_RELEASE(model);
}

void VolumeRegion::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<MultiMaterial4> >& polyVoxMesh, uint32_t lod)
{
	GP_ERROR("This function should never be called!"); //See note in header
}

void VolumeRegion::buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< Colour > >& polyVoxMesh, uint32_t lod)
{
	GP_ERROR("This function should never be called!"); //See note in header
}

void VolumeRegion::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal<GameplayMarchingCubesController< MultiMaterial4 >::MaterialType> >& polyVoxMesh, uint32_t lod)
{
	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<PositionMaterialNormal<GameplayMarchingCubesController< MultiMaterial4 >::MaterialType> >& vecVertices = polyVoxMesh.getVertices();
	const float* pVerticesConst = reinterpret_cast<const float*>(&vecVertices[0]);
	float* pVertices = const_cast<float*>(pVerticesConst);

	VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 4),
		VertexFormat::Element(VertexFormat::TEXCOORD0, 4),
		VertexFormat::Element(VertexFormat::NORMAL, 3)
    };

	//Create the vertex data in the expected format
	float* vertexData = new float[polyVoxMesh.getVertices().size() * 11]; //11 float per vertex
	float* ptr = vertexData;
	for(int i = 0; i < vecVertices.size(); i++)
	{
		*ptr = vecVertices[i].getPosition().getX(); ptr++;
		*ptr = vecVertices[i].getPosition().getY(); ptr++;
		*ptr = vecVertices[i].getPosition().getZ(); ptr++;
		*ptr = 1.0;  ptr++;

		// Material values range from 0 - getMaxMaterialValue() for each voxel. At the position
		// of the isosurface materials are not at their full intensity (they are at roughly half
		// because that's where the theshold is). We need to normalise the values to thier full range.
		Vector<8, float> matAsVec = vecVertices[i].getMaterial();
		matAsVec.normalise();

		*ptr = matAsVec.getElement(0); ptr++;
		*ptr = matAsVec.getElement(1); ptr++;
		*ptr = matAsVec.getElement(2); ptr++;
		*ptr = matAsVec.getElement(3); ptr++;
		
		*ptr = vecVertices[i].getNormal().getX(); ptr++;
		*ptr = vecVertices[i].getNormal().getY(); ptr++;
		*ptr = vecVertices[i].getNormal().getZ(); ptr++;
	}

    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), polyVoxMesh.getVertices().size(), false);
    /*if (mesh == NULL)
    {
        return NULL;
    }*/
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    mesh->setVertexData(vertexData, 0, polyVoxMesh.getVertices().size());
	mesh->setBoundingBox(BoundingBox(Vector3(0,0,0), Vector3(16, 16, 16)));
	delete[] vertexData;

	//Can get rid of this casting in the future? See https://github.com/blackberry/GamePlay/issues/267
	const std::vector<unsigned int>& vecIndices = polyVoxMesh.getIndices();
	const void* pIndicesConst = &vecIndices[0];
	void* pIndices = const_cast<void*>(pIndicesConst);
	MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX32, polyVoxMesh.getNoOfIndices());
	meshPart->setIndexData(pIndices, 0, vecIndices.size());

    Model* model = Model::create(mesh);
    SAFE_RELEASE(mesh);

	uint32_t* pLod = new uint32_t;
	*pLod = lod;

	mNode[lod]->setUserPointer(pLod, lodCleanupCallback);
	mNode[lod]->setModel(model);
	SAFE_RELEASE(model);
}

void VolumeRegion::setMaterial(const char* material)
{
	for(uint32_t lod = 0; lod < NoOfLodLevels; lod++)
	{
		if(mNode[lod]->getModel())
		{
			mNode[lod]->getModel()->setMaterial(material);
		}
	}
}