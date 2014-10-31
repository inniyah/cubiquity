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
	const uint32_t HighestMeshLevel = 2;

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
	class ClearWantedForRenderingVisitor
	{
	public:
		bool operator()(OctreeNode<VoxelType>* octreeNode)
		{
			octreeNode->mWantedForRendering = false;
			return true;
		}
	};

	template <typename VoxelType>
	class DetermineActiveNodesVisitor
	{
	public:
		DetermineActiveNodesVisitor(const Vector3F& viewPosition, float lodThreshold)
			:mViewPosition(viewPosition)
			, mLodThreshold(lodThreshold)
		{

		}

		bool operator()(OctreeNode<VoxelType>* octreeNode)
		{
			// FIXME - Should have an early out to set active to false if parent is false.

			OctreeNode<VoxelType>* parentNode = octreeNode->getParentNode();
			if (parentNode)
			{

				Vector3F regionCentre = static_cast<Vector3F>(parentNode->mRegion.getCentre());

				float distance = (mViewPosition - regionCentre).length();

				Vector3I diagonal = parentNode->mRegion.getUpperCorner() - parentNode->mRegion.getLowerCorner();
				float diagonalLength = diagonal.length(); // A measure of our regions size

				float projectedSize = diagonalLength / distance;

				// As we move far away only the highesy nodes will be larger than the threshold. But these may be too
				// high to ever generate meshes, so we set here a maximum height for which nodes can be set to inacive.
				bool active = (projectedSize > mLodThreshold) || (parentNode->mHeight >= HighestMeshLevel);

				octreeNode->setActive(active);
			}
			else
			{
				octreeNode->setActive(true);
			}

			return true; // Process children
		}

	private:
		const Vector3F& mViewPosition;
		float mLodThreshold;
	};

	template <typename VoxelType>
	class DetermineWantedForRenderingVisitor
	{
	public:
		DetermineWantedForRenderingVisitor(const Vector3F& viewPosition, float lodThreshold)
			:mViewPosition(viewPosition)
			,mLodThreshold(lodThreshold)
		{
		}

		bool operator()(OctreeNode<VoxelType>* octreeNode)
		{
			if(octreeNode->mHeight == 0)
			{
				octreeNode->mWantedForRendering = true;
				return false;
			}
			else
			{
				Vector3F regionCentre = static_cast<Vector3F>(octreeNode->mRegion.getCentre());

				float distance = (mViewPosition - regionCentre).length();

				Vector3I diagonal = octreeNode->mRegion.getUpperCorner() - octreeNode->mRegion.getLowerCorner();
				float diagonalLength = diagonal.length(); // A measure of our regions size

				float projectedSize = diagonalLength / distance;

				bool processChildren = ((projectedSize > mLodThreshold) || (octreeNode->mHeight > HighestMeshLevel)); //subtree height check prevents building LODs for node near the root.

				if(processChildren)
				{
					return true;
				}
				else
				{
					octreeNode->mWantedForRendering = true;
					return false;
				}
			}
		}

	private:
		const Vector3F& mViewPosition;
		float mLodThreshold;
	};

	template <typename VoxelType>
	class DetermineWhetherToRenderVisitor
	{
	public:
		bool operator()(OctreeNode<VoxelType>* octreeNode)
		{
			//At some point we should handle the issue that we might want to render but the mesh might not be ready.
			if (octreeNode->mRenderThisNode != octreeNode->mWantedForRendering)
			{
				octreeNode->mRenderThisNode = octreeNode->mWantedForRendering;
				octreeNode->mStructureLastChanged = Clock::getTimestamp();
			}
			return true;
		}
	};

	template <typename VoxelType>
	class Octree
	{
		friend class OctreeNode<VoxelType>;

	public:
		static const uint16_t InvalidNodeIndex = 0xFFFF;

		Octree(Volume<VoxelType>* volume, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize);
		~Octree();

		template<typename VisitorType>
		void acceptVisitor(VisitorType visitor) { visitNode(mRootNodeIndex, visitor); }

		OctreeNode<VoxelType>* getRootNode(void) { return mNodes[mRootNodeIndex]; }

		// This one feels hacky?
		OctreeNode<VoxelType>* getNodeFromIndex(uint16_t index) { return mNodes[index]; }

		void update(const Vector3F& viewPosition, float lodThreshold);

		void markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority);
		void markDataAsModified(const Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority);

		void buildOctreeNodeTree(uint16_t parent);

		concurrent_queue<typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType*, TaskSortCriterion> mFinishedSurfaceExtractionTasks;

	private:
		uint16_t createNode(Region region, uint16_t parent);

		template<typename VisitorType>
		void visitNode(uint16_t index, VisitorType visitor);

		void markAsModified(uint16_t index, int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority);
		void markAsModified(uint16_t index, const Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority);

		Timestamp Octree<VoxelType>::propagateTimestamps(uint16_t index);
		Timestamp Octree<VoxelType>::propagateMeshTimestamps(uint16_t index);

		void sceduleUpdateIfNeeded(uint16_t index, const Vector3F& viewPosition);

		bool Octree<VoxelType>::determineWhetherToRender(uint16_t index);

		//void determineWantedForRendering(uint16_t index, const Vector3F& viewPosition, float lodThreshold);

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
