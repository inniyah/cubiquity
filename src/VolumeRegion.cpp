#include "VolumeRegion.h"

#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

VolumeRegion::VolumeRegion(PolyVox::Region region)
	:mRegion(region)
{
	 mNode = Node::create();
}

VolumeRegion::~VolumeRegion()
{
	 SAFE_RELEASE(mNode);
}

void VolumeRegion::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& polyVoxMesh)
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
    model->setMaterial("res/PolyVox.material");
    SAFE_RELEASE(mesh);

	mNode->setModel(model);
}