#include "Volume.h"

#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

Volume::Volume()
	:mVolData(0)
	,mRootNode(0)
{
	mRootNode = Node::create();
	mVolumeRegion = new VolumeRegion(PolyVox::Region(Vector3DInt32(0,0,0), Vector3DInt32(32, 8, 32)));
	mRootNode->addChild(mVolumeRegion->mNode);
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

	mVolumeRegion->buildGraphicsMesh(polyVoxMesh);
}
