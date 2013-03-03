#include "Volume.h"

#include "ColouredCubicSurfaceExtractionTask.h"
#include "Octree.h"
#include "SmoothSurfaceExtractionTask.h"

#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceMesh.h"

#include <sstream>

template <typename VoxelType>
OctreeNode<VoxelType>::OctreeNode(PolyVox::Region region, OctreeNode* parentRegion, Octree<VoxelType>* octree)
	:mRegion(region)
	,parent(parentRegion)
	,mOctree(octree)
	,mWantedForRendering(false)
	,mRenderThisNode(false)
	,mLastSceduledForUpdate(Clock::getTimestamp()) // The order ofthese few initialisations is important
	,mMeshLastUpdated(Clock::getTimestamp())	   // to meke sure the node is set to an 'out of date' 
	,mDataLastModified(Clock::getTimestamp())      // state which will then try to update.
	,mPolyVoxMesh(0)
	,mGameEngineNode(0)
	,mLodLevel(0)
	,mLastSurfaceExtractionTask(0)
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
void OctreeNode<VoxelType>::markAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp)
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
						child->markAsModified(x, y, z, newTimeStamp);
					}
				}
			}
		}
	}
}

template <typename VoxelType>
void OctreeNode<VoxelType>::markAsModified(const PolyVox::Region& region, Timestamp newTimeStamp)
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
						child->markAsModified(region, newTimeStamp);
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
		PolyVox::Vector3DFloat regionCentre = static_cast<PolyVox::Vector3DFloat>(mRegion.getCentre());

		float distance = (viewPosition - regionCentre).length();

		PolyVox::Vector3DInt32 diagonal = mRegion.getUpperCorner() - mRegion.getLowerCorner();
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
void OctreeNode<VoxelType>::sceduleUpdateIfNeeded(void)
{
	if((isMeshUpToDate() == false) && (isSceduledForUpdate() == false) && ((mLastSurfaceExtractionTask == 0) || (mLastSurfaceExtractionTask->getState() != TaskStates::Pending)) && (mWantedForRendering))
	{
		mLastSceduledForUpdate = Clock::getTimestamp();

		mLastSurfaceExtractionTask = new VoxelTraits<VoxelType>::SurfaceExtractionTaskType(this, mOctree->mVolume->mPolyVoxVolume);

		gBackgroundTaskProcessor.addTask(mLastSurfaceExtractionTask);

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
					child->sceduleUpdateIfNeeded();
				}
			}
		}
	}
}

template <typename VoxelType>
bool OctreeNode<VoxelType>::updateFromCompletedTask(typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* completedTask)
{
	// There is sme chance that the volume data has been modified between the time that we started performing the
	// surface extraction and now. If the mesh is out of date then just discard it - a new one should be along soon.
	if(completedTask->mStartedProcessingTimestamp > mDataLastModified)
	{
		if(completedTask->mPolyVoxMesh->getNoOfIndices() > 0)
		{
			mPolyVoxMesh = completedTask->mPolyVoxMesh;
		}

		setMeshLastUpdated(Clock::getTimestamp());

		return true;
	}
	else
	{
		return false;
	}
}
