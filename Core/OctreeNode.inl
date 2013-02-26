#include "Volume.h"

#include "ColouredCubicSurfaceExtractionTask.h"
#include "Octree.h"
#include "SmoothSurfaceExtractionTask.h"

#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceMesh.h"

#include <sstream>

using namespace PolyVox;

template <typename VoxelType>
OctreeNode<VoxelType>::OctreeNode(PolyVox::Region region, OctreeNode* parentRegion, Octree<VoxelType>* octree)
	:mRegion(region)
	,parent(parentRegion)
	,mOctree(octree)
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

template <typename VoxelType>
OctreeNode<VoxelType>::~OctreeNode()
{
}

template <typename VoxelType>
void OctreeNode<VoxelType>::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	clearWantedForRendering();
	determineWantedForRendering(viewPosition, lodThreshold);

	sceduleForUpdate();

	determineWhetherToRender();
}

template <typename VoxelType>
void OctreeNode<VoxelType>::markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp)
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

template <typename VoxelType>
void OctreeNode<VoxelType>::markDataAsModified(const Region& region, Timestamp newTimeStamp)
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

template <typename VoxelType>
bool OctreeNode<VoxelType>::hasAnyChildren(void)
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

template <typename VoxelType>
bool OctreeNode<VoxelType>::allChildrenUpToDate(void)
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

template <typename VoxelType>
void OctreeNode<VoxelType>::clearWantedForRendering(void)
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

template <typename VoxelType>
void OctreeNode<VoxelType>::determineWantedForRendering(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
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

		if((projectedSize > lodThreshold) || (mLodLevel > 2)) //subtree height check prevents building LODs for node near the root.
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
							child->determineWantedForRendering(viewPosition, lodThreshold);
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

template <typename VoxelType>
void OctreeNode<VoxelType>::determineWhetherToRender(void)
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

template <typename VoxelType>
bool OctreeNode<VoxelType>::isMeshUpToDate(void)
{
	return mMeshLastUpdated > mDataLastModified;
}

template <typename VoxelType>
bool OctreeNode<VoxelType>::isSceduledForUpdate(void)
{
	//We are sceduled for an update if being sceduled was the most recent thing that happened.
	return (mLastSceduledForUpdate > mDataLastModified) && (mLastSceduledForUpdate > mMeshLastUpdated);
}

template <typename VoxelType>
void OctreeNode<VoxelType>::setMeshLastUpdated(Timestamp newTimeStamp)
{
	mMeshLastUpdated = newTimeStamp;
}

template <typename VoxelType>
void OctreeNode<VoxelType>::sceduleForUpdate(void)
{
	if((isMeshUpToDate() == false) && (isSceduledForUpdate() == false) && (mWantedForRendering))
	{
		mLastSceduledForUpdate = Clock::getTimestamp();

		VoxelTraits<VoxelType>::SurfaceExtractionTaskType* task = new VoxelTraits<VoxelType>::SurfaceExtractionTaskType(this, mOctree->mVolume->mPolyVoxVolume);

		gMainThreadTaskProcessor.addTask(task);

		//updateMeshImpl(octreeNode);
	}

	for(int iz = 0; iz < 2; iz++)
	{
		for(int iy = 0; iy < 2; iy++)
		{
			for(int ix = 0; ix < 2; ix++)
			{
				OctreeNode* child = children[ix][iy][iz];
				if(child)
				{
					child->sceduleForUpdate();
				}
			}
		}
	}
}
