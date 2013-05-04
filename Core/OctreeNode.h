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
		friend class Octree<VoxelType>;

	public:	
		OctreeNode(Region region, uint16_t parent, Octree<VoxelType>* octree);
		~OctreeNode();

		OctreeNode* getChildNode(uint32_t childX, uint32_t childY, uint32_t childZ);
		OctreeNode* getParentNode(void);

		bool isMeshUpToDate(void);
		bool isSceduledForUpdate(void);

		void setMeshLastUpdated(Timestamp newTimeStamp);

		void updateFromCompletedTask(typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* completedTask);

		Region mRegion;
		Timestamp mDataLastModified;
		Timestamp mMeshLastUpdated;
		Timestamp mLastSceduledForUpdate;

		Octree<VoxelType>* mOctree;

		// Use flags here?
		bool mWantedForRendering;
		bool mRenderThisNode;
		bool mExtractOnMainThread;

		const ::PolyVox::SurfaceMesh< typename VoxelTraits<VoxelType>::VertexType >* mPolyVoxMesh;

		void* mGameEngineNode;

		uint8_t mHeight; // Zero for leaf nodes.

		typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* mLastSurfaceExtractionTask;

		uint16_t mSelf;

	private:
		uint16_t mParent;
		uint16_t children[2][2][2];
	};
}

#include "OctreeNode.inl"

#endif //OCTREE_NODE_H_
