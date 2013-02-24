#include "Volume.h"

#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceMesh.h"

#include <sstream>

using namespace PolyVox;

template <typename VertexType>
OctreeNode<VertexType>::OctreeNode(PolyVox::Region region, OctreeNode* parentRegion)
	:mRegion(region)
	,parent(parentRegion)
	,mWantedForRendering(false)
	,mRenderThisNode(false)
	,mLastSceduledForUpdate(Clock::getTimestamp())
	,mMeshLastUpdated(Clock::getTimestamp())	
	,mDataLastModified(Clock::getTimestamp())
	,mPolyVoxMesh(0)
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

template <typename VertexType>
OctreeNode<VertexType>::~OctreeNode()
{
}

template <typename VertexType>
void OctreeNode<VertexType>::markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp)
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

template <typename VertexType>
void OctreeNode<VertexType>::markDataAsModified(const Region& region, Timestamp newTimeStamp)
{
	if(intersects(mRegion, region))
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
						child->markDataAsModified(region, newTimeStamp);
					}
				}
			}
		}
	}
}

template <typename VertexType>
bool OctreeNode<VertexType>::hasAnyChildren(void)
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

template <typename VertexType>
bool OctreeNode<VertexType>::allChildrenUpToDate(void)
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

template <typename VertexType>
void OctreeNode<VertexType>::clearWantedForRendering(void)
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

template <typename VertexType>
void OctreeNode<VertexType>::determineWantedForRendering(const PolyVox::Vector3DFloat& viewPosition, float threshold)
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

		if((projectedSize > threshold) || (mLodLevel > 2)) //subtree height check prevents building LODs for node near the root.
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

template <typename VertexType>
void OctreeNode<VertexType>::determineWhetherToRender(void)
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

template <typename VertexType>
bool OctreeNode<VertexType>::isMeshUpToDate(void)
{
	return mMeshLastUpdated > mDataLastModified;
}

template <typename VertexType>
bool OctreeNode<VertexType>::isSceduledForUpdate(void)
{
	//We are sceduled for an update if being sceduled was the most recent thing that happened.
	return (mLastSceduledForUpdate > mDataLastModified) && (mLastSceduledForUpdate > mMeshLastUpdated);
}

template <typename VertexType>
void OctreeNode<VertexType>::setMeshLastUpdated(Timestamp newTimeStamp)
{
	mMeshLastUpdated = newTimeStamp;
}
