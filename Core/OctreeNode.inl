#include "Volume.h"

#include "ColoredCubicSurfaceExtractionTask.h"
#include "Octree.h"
#include "SmoothSurfaceExtractionTask.h"

#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/Mesh.h"

#include <limits>
#include <sstream>

namespace Cubiquity
{
	template <typename VoxelType>
	OctreeNode<VoxelType>::OctreeNode(Region region, uint16_t parent, Octree<VoxelType>* octree)
		:mRegion(region)
		,mParent(parent)
		,mOctree(octree)
		,mWantedForRendering(false)
		,mRenderThisNode(false)
		,mExtractOnMainThread(false)
		,mActive(false)
		,mLastSceduledForUpdate(0) // The values of these few initialisations is important
		,mMeshLastChanged(1)	   // to make sure the node is set to an 'out of date' 
		,mDataLastModified(2)      // state which will then try to update.
		,mStructureLastChanged(3)
		,mPolyVoxMesh(0)
		,mHeight(0)
		,mLastSurfaceExtractionTask(0)
	{
		for(int z = 0; z < 2; z++)
		{
			for(int y = 0; y < 2; y++)
			{
				for(int x = 0; x < 2; x++)
				{
					children[x][y][z] = Octree<VoxelType>::InvalidNodeIndex;
				}
			}
		}
	}

	template <typename VoxelType>
	OctreeNode<VoxelType>::~OctreeNode()
	{
		delete mPolyVoxMesh;
	}

	template <typename VoxelType>
	OctreeNode<VoxelType>* OctreeNode<VoxelType>::getChildNode(uint32_t childX, uint32_t childY, uint32_t childZ)
	{
		return children[childX][childY][childZ] == Octree<VoxelType>::InvalidNodeIndex ? 0 : mOctree->mNodes[children[childX][childY][childZ]];
	}

	template <typename VoxelType>
	OctreeNode<VoxelType>* OctreeNode<VoxelType>::getParentNode(void)
	{
		return mParent == Octree<VoxelType>::InvalidNodeIndex ? 0 : mOctree->mNodes[mParent];
	}

	template <typename VoxelType>
	const ::PolyVox::Mesh< typename VoxelTraits<VoxelType>::VertexType, uint16_t >* OctreeNode<VoxelType>::getMesh(void)
	{
		return mPolyVoxMesh;
	}

	template <typename VoxelType>
	void OctreeNode<VoxelType>::setMesh(const ::PolyVox::Mesh< typename VoxelTraits<VoxelType>::VertexType, uint16_t >* mesh)
	{
		if (mPolyVoxMesh)
		{
			delete mPolyVoxMesh;
			mPolyVoxMesh = 0;
		}

		mPolyVoxMesh = mesh;

		mMeshLastChanged = Clock::getTimestamp();

		if (mPolyVoxMesh == 0)
		{
			// Force the mesh to be updated next time it is needed.
			mDataLastModified = Clock::getTimestamp();
		}
	}

	template <typename VoxelType>
	bool OctreeNode<VoxelType>::isActive(void)
	{
		return mActive;
	}

	template <typename VoxelType>
	void OctreeNode<VoxelType>::setActive(bool active)
	{
		if (mActive != active)
		{
			mActive = active;
			mStructureLastChanged = Clock::getTimestamp();

			if (mActive == false)
			{
				setMesh(0);
			}
		}
	}

	template <typename VoxelType>
	bool OctreeNode<VoxelType>::isMeshUpToDate(void)
	{
		return mMeshLastChanged > mDataLastModified;
	}

	template <typename VoxelType>
	bool OctreeNode<VoxelType>::isSceduledForUpdate(void)
	{
		//We are sceduled for an update if being sceduled was the most recent thing that happened.
		return (mLastSceduledForUpdate > mDataLastModified) && (mLastSceduledForUpdate > mMeshLastChanged);
	}

	template <typename VoxelType>
	void OctreeNode<VoxelType>::updateFromCompletedTask(typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* completedTask)
	{
		// Assign a new mesh if available
		/*if(completedTask->mPolyVoxMesh->getNoOfIndices() > 0)
		{*/
			setMesh(completedTask->mPolyVoxMesh);
			completedTask->mOwnMesh = false; // So the task doesn't delete the mesh
		/*}
		else // Otherwise it will just be deleted.
		{
			setMesh(0);
		}*/
	}
}
