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

		const ::PolyVox::Mesh< typename VoxelTraits<VoxelType>::VertexType, uint16_t >* getMesh(void);
		void setMesh(const ::PolyVox::Mesh< typename VoxelTraits<VoxelType>::VertexType, uint16_t >* mesh);

		bool isActive(void);
		void setActive(bool active);

		bool isMeshUpToDate(void);
		bool isSceduledForUpdate(void);

		void updateFromCompletedTask(typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* completedTask);

		Region mRegion;
		Timestamp mDataLastModified;
		Timestamp mLastSceduledForUpdate;

		Timestamp mStructureLastChanged;
		Timestamp mStructureLastChangedRecursive;

		Timestamp mMeshLastChanged;
		Timestamp mMeshLastChangedRecursive;

		Octree<VoxelType>* mOctree;

		// Use flags here?
		//bool mWantedForRendering;
		bool mRenderThisNode;
		bool mCanRenderNodeOrChildren;
		bool mExtractOnMainThread;
		bool mActive;
		bool mIsLeaf;

		uint8_t mHeight; // Zero for leaf nodes.

		typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* mLastSurfaceExtractionTask;

		uint16_t mSelf;

	private:
		uint16_t mParent;
		uint16_t children[2][2][2];

		const ::PolyVox::Mesh< typename VoxelTraits<VoxelType>::VertexType, uint16_t >* mPolyVoxMesh;
	};
}

#include "OctreeNode.inl"

#endif //OCTREE_NODE_H_
