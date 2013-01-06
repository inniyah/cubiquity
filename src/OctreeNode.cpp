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
	,mSmoothPolyVoxMesh(0)
	,mCubicPolyVoxMesh(0)
	,mGameEngineNode(0)
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
	GP_ERROR("This function should never be called!"); //See note in header
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< Colour > >* polyVoxMesh)
{
	GP_ERROR("This function should never be called!"); //See note in header
}

void OctreeNode::buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal<GameplayMarchingCubesController< MultiMaterial4 >::MaterialType> >* polyVoxMesh)
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

uint32_t OctreeNode::subtreeHeight(void)
{
	uint32_t maxChildHeight = 0;
	for(int iz = 0; iz < 2; iz++)
	{
		for(int iy = 0; iy < 2; iy++)
		{
			for(int ix = 0; ix < 2; ix++)
			{
				OctreeNode* child = children[ix][iy][iz];
				if(child)
				{
					maxChildHeight = std::max(maxChildHeight, child->subtreeHeight());
				}
			}
		}
	}

	return maxChildHeight + 1;
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
