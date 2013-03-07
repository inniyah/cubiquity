#ifndef CUBIQUITY_OCTREE_H_
#define CUBIQUITY_OCTREE_H_

#include "Clock.h"
#include "ConcurrentQueue.h"
#include "CubiquityForwardDeclarations.h"
#include "Task.h"
#include "UpdatePriorities.h"
#include "VoxelTraits.h"

namespace Cubiquity
{
	namespace OctreeConstructionModes
	{
		enum OctreeConstructionMode
		{
			BoundVoxels = 0,
			BoundCells = 1
		};
	}
	typedef OctreeConstructionModes::OctreeConstructionMode OctreeConstructionMode;

	template <typename VoxelType>
	class Octree
	{
	public:
		Octree(Volume<VoxelType>* volume, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize);

		void update(const ::PolyVox::Vector3DFloat& viewPosition, float lodThreshold);

		void markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority);
		void markDataAsModified(const ::PolyVox::Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority);

		void buildOctreeNodeTree(OctreeNode< VoxelType >* parent, const ::PolyVox::Region& regionToCover, OctreeConstructionMode octreeConstructionMode);

		Volume<VoxelType>* mVolume;
		OctreeNode<VoxelType>* mRootOctreeNode;
		const unsigned int mBaseNodeSize;

		concurrent_queue<typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType*, TaskSortCriterion> mFinishedSurfaceExtractionTasks;
	};
}

#include "Octree.inl"

#endif //CUBIQUITY_OCTREE_H_
