#ifndef CUBIQUITY_OCTREE_H_
#define CUBIQUITY_OCTREE_H_

#include "Clock.h"
#include "ConcurrentQueue.h"
#include "CubiquityForwardDeclarations.h"
#include "Region.h"
#include "Task.h"
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
		friend class OctreeNode<VoxelType>;

	public:
		static const uint16_t InvalidNodeIndex = 0xFFFF;

		Octree(Volume<VoxelType>* volume, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize);
		~Octree();

		template<typename VisitorType>
		void acceptVisitor(VisitorType visitor) { visitNode(getRootNode(), visitor); }

		OctreeNode<VoxelType>* getRootNode(void) { return mNodes[mRootNodeIndex]; }

		Volume<VoxelType>* getVolume(void) { return mVolume; }

		// This one feels hacky?
		OctreeNode<VoxelType>* getNodeFromIndex(uint16_t index) { return mNodes[index]; }

		bool update(const Vector3F& viewPosition, float lodThreshold);

		void markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp);
		void markDataAsModified(const Region& region, Timestamp newTimeStamp);

		void buildOctreeNodeTree(uint16_t parent);
		void determineActiveNodes(OctreeNode<VoxelType>* octreeNode, const Vector3F& viewPosition, float lodThreshold);

		concurrent_queue<typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType*, TaskSortCriterion> mFinishedSurfaceExtractionTasks;

		// Note that the maximum LOD refers to the *highest* level of detail, which is actually the *smallest* hieght in
		// the octree. If confused, think how texture mipmapping works, where the highest MIP levels have the smallest  
		// images and the lowest amount of detail. Level zero is the raw voxel data and succesive levels downsample it.
		int32_t mMaximumLOD;
		int32_t mMinimumLOD;

	private:
		uint16_t createNode(Region region, uint16_t parent);

		template<typename VisitorType>
		void visitNode(OctreeNode<VoxelType>* node, VisitorType& visitor);

		void markAsModified(uint16_t index, int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp);
		void markAsModified(uint16_t index, const Region& region, Timestamp newTimeStamp);

		Timestamp Octree<VoxelType>::propagateTimestamps(uint16_t index);

		void scheduleUpdateIfNeeded(OctreeNode<VoxelType>* node, const Vector3F& viewPosition);

		void Octree<VoxelType>::determineWhetherToRenderNode(uint16_t index);

		std::vector< OctreeNode<VoxelType>*> mNodes;

		uint16_t mRootNodeIndex;
		const unsigned int mBaseNodeSize;

		Volume<VoxelType>* mVolume;		

		// The extent of the octree may be significantly larger than the volume, but we only want to
		// create nodes which actually overlap the volume (otherwise they are guarenteed to be empty).
		Region mRegionToCover;

		OctreeConstructionMode mOctreeConstructionMode;
	};
}

#include "Octree.inl"

#endif //CUBIQUITY_OCTREE_H_
