#ifndef CUBIQUITY_OCTREE_H_
#define CUBIQUITY_OCTREE_H_

#include "Clock.h"
#include "ConcurrentQueue.h"
#include "CubiquityForwardDeclarations.h"
#include "Region.h"
#include "Task.h"
#include "UpdatePriorities.h"
#include "Vector.h"
#include "VoxelTraits.h"

#include <vector>

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

		uint16_t createNode(Region region, uint16_t parent);

		void update(const Vector3F& viewPosition, float lodThreshold);

		void markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority);
		void markDataAsModified(const Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority);

		void buildOctreeNodeTree(uint16_t parent, const Region& regionToCover, OctreeConstructionMode octreeConstructionMode);

		Volume<VoxelType>* mVolume;
		uint16_t mRootOctreeNode;
		const unsigned int mBaseNodeSize;

		std::vector< OctreeNode<VoxelType>*> mNodes;

		concurrent_queue<typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType*, TaskSortCriterion> mFinishedSurfaceExtractionTasks;

	private:
		void clearWantedForRendering(uint16_t index);
		void determineWantedForRendering(uint16_t index, const Vector3F& viewPosition, float lodThreshold);
		void determineWhetherToRender(uint16_t index);

		void markAsModified(uint16_t index, int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority);
		void markAsModified(uint16_t index, const Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority);

		void sceduleUpdateIfNeeded(uint16_t index, const Vector3F& viewPosition);
	};
}

#include "Octree.inl"

#endif //CUBIQUITY_OCTREE_H_
