#include "OctreeNode.h"

#include "Volume.h"

#include <sstream>

using namespace PolyVox;

OctreeNode::OctreeNode(PolyVox::Region region, OctreeNode* parentRegion)
	:mRegion(region)
	//,mIsMeshUpToDate(false)
	,parent(parentRegion)
	,mWantedForRendering(false)
	,mRenderThisNode(false)
	,mMeshLastUpdated(0)
	,mDataLastModified(1) //Is this ok?
	,mSmoothPolyVoxMesh(0)
	,mCubicPolyVoxMesh(0)
	,mGameEngineNode(0)
	,mLodLevel(0)
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

	if(parent)
	{
		POLYVOX_ASSERT(parent->mLodLevel < 100, "LOD level has gone below zero and wrapped around.");
		mLodLevel = parent->mLodLevel-1;
	}
}

OctreeNode::~OctreeNode()
{
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<Colour> >* polyVoxMesh)
{
	mCubicPolyVoxMesh = polyVoxMesh;
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<MultiMaterial4> >* polyVoxMesh)
{
	POLYVOX_ASSERT(false, "This function should never be called!"); //See note in header
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< Colour > >* polyVoxMesh)
{
	POLYVOX_ASSERT(false, "This function should never be called!"); //See note in header
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal<MultiMaterialMarchingCubesController< MultiMaterial4 >::MaterialType> >* polyVoxMesh)
{
	mSmoothPolyVoxMesh = polyVoxMesh;
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

void OctreeNode::determineWantedForRendering(const PolyVox::Vector3DFloat& viewPosition, float threshold)
{
	if(mLodLevel == 0)
	{
		mWantedForRendering = true;
	}
	else
	{
		Vector3DFloat regionCentre = static_cast<Vector3DFloat>(mRegion.getCentre());

		float distance = (viewPosition - regionCentre).length();

		Vector3DInt32 diagonal = mRegion.getUpperCorner() - mRegion.getLowerCorner();
		float diagonalLength = diagonal.length(); // A measure of our regions size

		float projectedSize = diagonalLength / distance;

		if((projectedSize > threshold) || (mLodLevel > 0)) //subtree height check prevents building LODs for node near the root.
		{
			for(int iz = 0; iz < 2; iz++)
			{
				for(int iy = 0; iy < 2; iy++)
				{
					for(int ix = 0; ix < 2; ix++)
					{
						OctreeNode* child = children[ix][iy][iz];
						if(child)
						{
							child->determineWantedForRendering(viewPosition, threshold);
						}
					}
				}
			}
		}
		else
		{
			mWantedForRendering = true;
		}
	}
}

void OctreeNode::determineWhetherToRender(void)
{
	//At some point we should handle the issue that we might want to render but the mesh might not be ready.
	mRenderThisNode = mWantedForRendering;

	for(int iz = 0; iz < 2; iz++)
	{
		for(int iy = 0; iy < 2; iy++)
		{
			for(int ix = 0; ix < 2; ix++)
			{
				OctreeNode* child = children[ix][iy][iz];
				if(child)
				{
					child->determineWhetherToRender();
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
