#ifndef OCTREE_NODE_H_
#define OCTREE_NODE_H_

#include "Clock.h"
#include "CubiquityForwardDeclarations.h"
#include "Region.h"
#include "UpdatePriorities.h"
#include "Vector.h"
#include "VoxelTraits.h"

namespace Cubiquity
{
	template <typename VoxelType>
	class OctreeNode
	{
	public:	
		OctreeNode(Region region, OctreeNode* parentRegion, Octree<VoxelType>* octree);
		~OctreeNode();

		void markAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority);
		void markAsModified(const Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority);

		void clearWantedForRendering(void);
		void determineWantedForRendering(const Vector3F& viewPosition, float lodThreshold);
		void determineWhetherToRender(void);

		bool hasAnyChildren(void);

		bool allChildrenUpToDate(void);

		bool isMeshUpToDate(void);
		bool isSceduledForUpdate(void);

		void setMeshLastUpdated(Timestamp newTimeStamp);

		void sceduleUpdateIfNeeded(const Vector3F& viewPosition);

		void updateFromCompletedTask(typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* completedTask);

		Region mRegion;
		Timestamp mDataLastModified;
		Timestamp mMeshLastUpdated;
		Timestamp mLastSceduledForUpdate;

		OctreeNode* parent;
		OctreeNode* children[2][2][2];

		Octree<VoxelType>* mOctree;

		// Use flags here?
		bool mWantedForRendering;
		bool mRenderThisNode;
		bool mExtractOnMainThread;

		const ::PolyVox::SurfaceMesh< typename VoxelTraits<VoxelType>::VertexType >* mPolyVoxMesh;

		void* mGameEngineNode;

		uint8_t mLodLevel; // Zero for leaf nodes.

		typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* mLastSurfaceExtractionTask;
	};
}

#include "OctreeNode.inl"

#endif //OCTREE_NODE_H_
