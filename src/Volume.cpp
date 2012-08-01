#include "Volume.h"

#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

Volume::Volume()
	:mVolData(0)
	,mRootNode(0)
{
	mRootNode = Node::create();
}

Volume::~Volume()
{
	SAFE_RELEASE(mRootNode);
}

Volume* Volume::create()
{
	Volume* volume = new Volume();
	volume->loadData();
	volume->updateMeshes();
	return volume;
}

Node* Volume::getRootNode()
{
	return mRootNode;
}

void Volume::setVoxelAt(int x, int y, int z, PolyVox::Material8 value)
{
	mVolData->setVoxelAt(x, y, z, value);
}

void Volume::loadData()
{
	mVolData = new SimpleVolume<Material8>(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(32, 8, 32)));

	//This vector hold the position of the center of the volume
	float fRadius = 6.0f;
	Vector3DFloat v3dVolCenter(mVolData->getWidth() / 2, mVolData->getHeight() / 2, mVolData->getDepth() / 2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < mVolData->getWidth(); z++)
	{
		for (int y = 0; y < mVolData->getHeight(); y++)
		{
			for (int x = 0; x < mVolData->getDepth(); x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x,y,z);	
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				uint8_t uMaterial = 1;

				//If the current voxel is less than 'radius' units from the center then we make it solid.
				if((x%2==0) && (z%2==0))
				{
					//Our new density value
					uMaterial = 0;
				}

				//Get the old voxel
				Material8 voxel = mVolData->getVoxelAt(x,y,z);

				//Modify the density and material
				voxel.setMaterial(uMaterial);

				//Wrte the voxel value into the volume	
				setVoxelAt(x, y, z, voxel);
			}
		}
	}
}

void Volume::updateMeshes()
{
	//Extract the surface
	SurfaceMesh<PositionMaterial> polyVoxMesh;
	CubicSurfaceExtractor< SimpleVolume<Material8> > surfaceExtractor(mVolData, mVolData->getEnclosingRegion(), &polyVoxMesh);
	surfaceExtractor.execute();

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

	mRootNode->setModel(model);
}
