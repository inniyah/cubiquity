#include "Volume.h"

#include "ColouredCubicSurfaceExtractionTask.h"
#include "Octree.h"
#include "SmoothSurfaceExtractionTask.h"

#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceMesh.h"

#include <limits>
#include <sstream>

template <typename VoxelType>
OctreeNode<VoxelType>::OctreeNode(PolyVox::Region region, OctreeNode* parentRegion, Octree<VoxelType>* octree)
	:mRegion(region)
	,parent(parentRegion)
	,mOctree(octree)
	,mWantedForRendering(false)
	,mRenderThisNode(false)
	,mExtractOnMainThread(false)
	,mLastSceduledForUpdate(Clock::getTimestamp()) // The order of these few initialisations is important
	,mMeshLastUpdated(Clock::getTimestamp())	   // to make sure the node is set to an 'out of date' 
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
void OctreeNode<VoxelType>::markAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority)
{
	// Note - Can't this function just call the other version?

	if(mRegion.containsPoint(x, y, z, -1)) //FIXME - Think if we really need this border.
	{
		//mIsMeshUpToDate = false;
		mDataLastModified = newTimeStamp;

		// Note: If DontUpdate was passed (an invalid choice) it will end up on the background thread.
		// Also we maintain mExtractOnMainThread if it was already set.
		mExtractOnMainThread = mExtractOnMainThread || (updatePriority == UpdatePriorities::Immediate);

		for(int iz = 0; iz < 2; iz++)
		{
			for(int iy = 0; iy < 2; iy++)
			{
				for(int ix = 0; ix < 2; ix++)
				{
					OctreeNode* child = children[ix][iy][iz];
					if(child)
					{
						child->markAsModified(x, y, z, newTimeStamp, updatePriority);
					}
				}
			}
		}
	}
}

template <typename VoxelType>
void OctreeNode<VoxelType>::markAsModified(const PolyVox::Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority)
{
	if(intersects(mRegion, region))
	{
		//mIsMeshUpToDate = false;
		mDataLastModified = newTimeStamp;

		// Note: If DontUpdate was passed (an invalid choice) it will end up on the background thread.
		mExtractOnMainThread = (updatePriority == UpdatePriorities::Immediate);

		for(int iz = 0; iz < 2; iz++)
		{
			for(int iy = 0; iy < 2; iy++)
			{
				for(int ix = 0; ix < 2; ix++)
				{
					OctreeNode* child = children[ix][iy][iz];
					if(child)
					{
						child->markAsModified(region, newTimeStamp, updatePriority);
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
void OctreeNode<VoxelType>::sceduleUpdateIfNeeded(const PolyVox::Vector3DFloat& viewPosition)
{
	if((isMeshUpToDate() == false) && (isSceduledForUpdate() == false) && ((mLastSurfaceExtractionTask == 0) || (mLastSurfaceExtractionTask->mProcessingStartedTimestamp < Clock::getTimestamp())) && (mWantedForRendering))
	{
		mLastSceduledForUpdate = Clock::getTimestamp();

		mLastSurfaceExtractionTask = new VoxelTraits<VoxelType>::SurfaceExtractionTaskType(this, mOctree->mVolume->mPolyVoxVolume);

		// If the node was rendered last frame then this update is probably the result of an editing operation, rather than
		// the node only just becoming visible. For editing operations it is important to process them immediatly so that we
		// don't see temporary cracks in the mesh as different parts up updated at different times.
		if(mExtractOnMainThread) //This flag should still be set from last frame.
		{
			// We're going to process immediatly, but the completed task will still get queued in the finished
			// queue, and we want to make sure it's the first out. So we still set a priority and make it high.
			mLastSurfaceExtractionTask->mPriority = std::numeric_limits<uint32_t>::max();
			gMainThreadTaskProcessor.addTask(mLastSurfaceExtractionTask);
		}
		else
		{
			// Note: tasks get sorted by their distance from the camera at the time they are added. If we
			// want to account for the camera moving then we would have to sort the task queue each frame.
			PolyVox::Vector3DFloat regionCentre = static_cast<PolyVox::Vector3DFloat>(mRegion.getCentre());
			float distance = (viewPosition - regionCentre).length(); //We don't use distance squared to keep the values smaller
			mLastSurfaceExtractionTask->mPriority = std::numeric_limits<uint32_t>::max() - static_cast<uint32_t>(distance);
			gBackgroundTaskProcessor.addTask(mLastSurfaceExtractionTask);
		}

		// Clear this flag otherwise this node will always be done on the main thread.
		mExtractOnMainThread = false;
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
					child->sceduleUpdateIfNeeded(viewPosition);
				}
			}
		}
	}
}

template <typename VoxelType>
void OctreeNode<VoxelType>::updateFromCompletedTask(typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* completedTask)
{
	if(completedTask->mPolyVoxMesh->getNoOfIndices() > 0)
	{
		mPolyVoxMesh = completedTask->mPolyVoxMesh;
	}

	setMeshLastUpdated(Clock::getTimestamp());
}
