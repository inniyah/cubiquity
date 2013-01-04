#include "OctreeNode.h"

#include "Volume.h"

#include "MeshPart.h"

#include <sstream>

using namespace gameplay;
using namespace PolyVox;

OctreeNode::OctreeNode(PolyVox::Region region, OctreeNode* parentRegion)
	:mRegion(region)
	//,mIsMeshUpToDate(false)
	,parent(parentRegion)
	,mWantedForRendering(false)
	,mMeshLastUpdated(0)
	,mDataLastModified(1) //Is this ok?
{
	for(int z = 0; z < 2; z++)
	{
		for(int y = 0; y < 2; y++)
		{
			for(int x = 0; x < 2; x++)
			{
				children[x][y][z] = 0;
			}
		}
	}

	std::stringstream ss;
	ss << "OctreeNodeNode(" << mRegion.getLowerCorner().getX() << "," << mRegion.getLowerCorner().getY() << "," << mRegion.getLowerCorner().getZ() << ")";
	mNode = Node::create(ss.str().c_str());
	if(parentRegion)
	{
		parentRegion->mNode->addChild(mNode);

		Vector3DInt32 translation = mRegion.getLowerCorner() - parentRegion->mRegion.getLowerCorner();

		mNode->setTranslation(translation.getX(), translation.getY(), translation.getZ());
	}
	mNode->setUserPointer(this);
}

OctreeNode::~OctreeNode()
{
	SAFE_RELEASE(mNode);
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<Colour> >& polyVoxMesh)
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

	mNode->setModel(model);
	SAFE_RELEASE(model);
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<MultiMaterial4> >& polyVoxMesh)
{
	GP_ERROR("This function should never be called!"); //See note in header
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< Colour > >& polyVoxMesh)
{
	GP_ERROR("This function should never be called!"); //See note in header
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal<GameplayMarchingCubesController< MultiMaterial4 >::MaterialType> >& polyVoxMesh)
{
	Model* model = buildModelFromPolyVoxMesh(polyVoxMesh);
	mNode->setModel(model);
	SAFE_RELEASE(model);

	//Now set up the physics
	const std::vector<PositionMaterialNormal<GameplayMarchingCubesController< MultiMaterial4 >::MaterialType> >& vecVertices = polyVoxMesh.getVertices();
	const std::vector<unsigned int>& vecIndices = polyVoxMesh.getIndices();
	float* vertexData = new float[polyVoxMesh.getVertices().size() * 3];

	unsigned int* physicsIndices = new unsigned int [vecIndices.size()];
	for(int ct = 0; ct < vecIndices.size(); ct++)
	{
		physicsIndices[ct] = vecIndices[ct];
	}

	float* ptr = vertexData;
	for(int i = 0; i < vecVertices.size(); i++)
	{
		// Position stored in x,y,z components.
		*ptr = vecVertices[i].getPosition().getX(); ptr++;
		*ptr = vecVertices[i].getPosition().getY(); ptr++;
		*ptr = vecVertices[i].getPosition().getZ(); ptr++;
	}

	//Putting the physics mesh on LOD 0.
	PhysicsRigidBody::Parameters groundParams;
	groundParams.mass = 0.0f;
	mNode->setCollisionObject(PhysicsCollisionObject::RIGID_BODY, PhysicsCollisionShape::custom(vertexData, polyVoxMesh.getVertices().size(), physicsIndices, vecIndices.size()), &groundParams);
}

void OctreeNode::setMaterial(const char* material)
{
	if(mNode->getModel())
	{
		mNode->getModel()->setMaterial(material);
	}
}

void OctreeNode::markDataAsModified(int32_t x, int32_t y, int32_t z, uint32_t newTimeStamp)
{
	if(mRegion.containsPoint(x, y, z, -1)) //FIXME - Think if we really need this border.
	{
		//mIsMeshUpToDate = false;
		mDataLastModified = newTimeStamp;

		for(int iz = 0; iz < 2; iz++)
		{
			for(int iy = 0; iy < 2; iy++)
			{
				for(int ix = 0; ix < 2; ix++)
				{
					OctreeNode* child = children[ix][iy][iz];
					if(child)
					{
						child->markDataAsModified(x, y, z, newTimeStamp);
					}
				}
			}
		}
	}
}

uint32_t OctreeNode::depth(void)
{
	if(parent)
	{
		return parent->depth() + 1;
	}
	else
	{
		return 0;
	}
}

bool OctreeNode::hasAnyChildren(void)
{
	for(int z = 0; z < 2; z++)
	{
		for(int y = 0; y < 2; y++)
		{
			for(int x = 0; x < 2; x++)
			{
				if(children[x][y][z] != 0)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool OctreeNode::allChildrenUpToDate(void)
{
	for(int z = 0; z < 2; z++)
	{
		for(int y = 0; y < 2; y++)
		{
			for(int x = 0; x < 2; x++)
			{
				if(children[x][y][z] != 0)
				{
					if(children[x][y][z]->isMeshUpToDate() == false)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

void OctreeNode::clearWantedForRendering(void)
{
	mWantedForRendering = false;

	for(int iz = 0; iz < 2; iz++)
	{
		for(int iy = 0; iy < 2; iy++)
		{
			for(int ix = 0; ix < 2; ix++)
			{
				OctreeNode* child = children[ix][iy][iz];
				if(child)
				{
					child->clearWantedForRendering();
				}
			}
		}
	}
}

bool OctreeNode::isMeshUpToDate(void)
{
	return mMeshLastUpdated > mDataLastModified;
}

void OctreeNode::setMeshLastUpdated(uint32_t newTimeStamp)
{
	mMeshLastUpdated = newTimeStamp;
}

gameplay::Model* OctreeNode::buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< GameplayMarchingCubesController< MultiMaterial4 >::MaterialType > >& polyVoxMesh)
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
		Vector<4, float> matAsVec = vecVertices[i].getMaterial();
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

	return model;
}