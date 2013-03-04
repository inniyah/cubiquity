#ifndef OCTREE_NODE_H_
#define OCTREE_NODE_H_

#include "Clock.h"
#include "CubiquityForwardDeclarations.h"
#include "VoxelTraits.h"

template <typename VoxelType>
class OctreeNode
{
public:	
	OctreeNode(PolyVox::Region region, OctreeNode* parentRegion, Octree<VoxelType>* octree);
	~OctreeNode();

	void markAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp);
	void markAsModified(const PolyVox::Region& region, Timestamp newTimeStamp);

	void clearWantedForRendering(void);
	void determineWantedForRendering(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	void determineWhetherToRender(void);

	bool hasAnyChildren(void);

	bool allChildrenUpToDate(void);

	bool isMeshUpToDate(void);
	bool isSceduledForUpdate(void);

	void setMeshLastUpdated(Timestamp newTimeStamp);

	void sceduleUpdateIfNeeded(void);

	void updateFromCompletedTask(typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* completedTask);

	PolyVox::Region mRegion;
	Timestamp mDataLastModified;
	Timestamp mMeshLastUpdated;
	Timestamp mLastSceduledForUpdate;

	OctreeNode* parent;
	OctreeNode* children[2][2][2];

	Octree<VoxelType>* mOctree;

	bool mWantedForRendering;
	bool mRenderThisNode;

	const PolyVox::SurfaceMesh< typename VoxelTraits<VoxelType>::VertexType >* mPolyVoxMesh;

	void* mGameEngineNode;

	uint8_t mLodLevel; // Zero for leaf nodes.

	typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* mLastSurfaceExtractionTask;
};

#include "OctreeNode.inl"

#endif //OCTREE_NODE_H_
