#include "OctreeNode.h"
#include "Volume.h"

namespace Cubiquity
{
	template <typename VoxelType>
	Octree<VoxelType>::Octree(Volume<VoxelType>* volume, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize)
		:mVolume(volume)
		,mRootOctreeNode(0)
		,mBaseNodeSize(baseNodeSize)
	{
		Region regionToCover(mVolume->getEnclosingRegion());
		if(octreeConstructionMode == OctreeConstructionModes::BoundVoxels)
		{
			regionToCover.shiftUpperCorner(1, 1, 1);
		}
		else if(octreeConstructionMode == OctreeConstructionModes::BoundCells)
		{
			regionToCover.shiftLowerCorner(-1, -1, -1);
			regionToCover.shiftUpperCorner(1, 1, 1);
		}

		POLYVOX_ASSERT(::PolyVox::isPowerOf2(mBaseNodeSize), "Node size must be a power of two");

		uint32_t largestVolumeDimension = std::max(regionToCover.getWidthInVoxels(), std::max(regionToCover.getHeightInVoxels(), regionToCover.getDepthInVoxels()));
		if(octreeConstructionMode == OctreeConstructionModes::BoundCells)
		{
			largestVolumeDimension--;
		}

		uint32_t octreeTargetSize = ::PolyVox::upperPowerOfTwo(largestVolumeDimension);

		uint8_t noOfLodLevels = logBase2((octreeTargetSize) / mBaseNodeSize) + 1;

		uint32_t regionToCoverWidth = (octreeConstructionMode == OctreeConstructionModes::BoundCells) ? regionToCover.getWidthInCells() : regionToCover.getWidthInVoxels();
		uint32_t regionToCoverHeight = (octreeConstructionMode == OctreeConstructionModes::BoundCells) ? regionToCover.getHeightInCells() : regionToCover.getHeightInVoxels();
		uint32_t regionToCoverDepth = (octreeConstructionMode == OctreeConstructionModes::BoundCells) ? regionToCover.getDepthInCells() : regionToCover.getDepthInVoxels();

		uint32_t widthIncrease = octreeTargetSize - regionToCoverWidth;
		uint32_t heightIncrease = octreeTargetSize - regionToCoverHeight;
		uint32_t depthIncrease = octreeTargetSize - regionToCoverDepth;

		Region octreeRegion(regionToCover);
	
		if(widthIncrease % 2 == 1)
		{
			octreeRegion.setUpperX(octreeRegion.getUpperX() + 1);
			widthIncrease--;
		}

		if(heightIncrease % 2 == 1)
		{
			octreeRegion.setUpperY(octreeRegion.getUpperY() + 1);
			heightIncrease--;
		}
		if(depthIncrease % 2 == 1)
		{
			octreeRegion.setUpperZ(octreeRegion.getUpperZ() + 1);
			depthIncrease--;
		}

		octreeRegion.grow(widthIncrease / 2, heightIncrease / 2, depthIncrease / 2);

		mNodes.push_back(0); //DUMMY NODE AT ZERO - HACK!!
		mRootOctreeNode = createNode(octreeRegion, 0);
		mNodes[mRootOctreeNode]->mLodLevel = noOfLodLevels - 1;

		buildOctreeNodeTree(mRootOctreeNode, regionToCover, octreeConstructionMode);
	}

	template <typename VoxelType>
	uint16_t Octree<VoxelType>::createNode(Region region, uint16_t parent)
	{
		OctreeNode< VoxelType >* node = new OctreeNode< VoxelType >(region, parent, this);
		mNodes.push_back(node);
		POLYVOX_ASSERT(mNodes.size() <= std::numeric_limits<uint16_t>::max(), "Too many octree nodes!");
		uint16_t index = mNodes.size() - 1;
		return index;
	}

	template <typename VoxelType>
	void Octree<VoxelType>::update(const Vector3F& viewPosition, float lodThreshold)
	{
		clearWantedForRendering(mRootOctreeNode);
		determineWantedForRendering(mRootOctreeNode, viewPosition, lodThreshold);

		mNodes[mRootOctreeNode]->sceduleUpdateIfNeeded(viewPosition);


		// Make sure any surface extraction tasks which were scheduled on the main thread get processed before we determine what to render.
		gMainThreadTaskProcessor.processAllTasks(); //Doesn't really belong here

		// This will include tasks from both the background and main threads.
		while(!mFinishedSurfaceExtractionTasks.empty())
		{
			VoxelTraits<VoxelType>::SurfaceExtractionTaskType* task;
			mFinishedSurfaceExtractionTasks.wait_and_pop(task);

			task->mOctreeNode->updateFromCompletedTask(task);

			if(task->mOctreeNode->mLastSurfaceExtractionTask == task)
			{
				task->mOctreeNode->mLastSurfaceExtractionTask = 0;
			}

			delete task;
		}

		determineWhetherToRender(mRootOctreeNode);
	}

	template <typename VoxelType>
	void Octree<VoxelType>::markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority)
	{
		markAsModified(mRootOctreeNode, x, y, z, newTimeStamp, updatePriority);
	}

	template <typename VoxelType>
	void Octree<VoxelType>::markDataAsModified(const Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority)
	{
		markAsModified(mRootOctreeNode, region, newTimeStamp, updatePriority);
	}

	template <typename VoxelType>
	void Octree<VoxelType>::buildOctreeNodeTree(uint16_t parent, const Region& regionToCover, OctreeConstructionMode octreeConstructionMode)
	{
		POLYVOX_ASSERT(mNodes[parent]->mRegion.getWidthInVoxels() == mNodes[parent]->mRegion.getHeightInVoxels(), "Region must be cubic");
		POLYVOX_ASSERT(mNodes[parent]->mRegion.getWidthInVoxels() == mNodes[parent]->mRegion.getDepthInVoxels(), "Region must be cubic");

		//We know that width/height/depth are all the same.
		uint32_t parentSize = static_cast<uint32_t>((octreeConstructionMode == OctreeConstructionModes::BoundCells) ? mNodes[parent]->mRegion.getWidthInCells() : mNodes[parent]->mRegion.getWidthInVoxels());

		if(parentSize > mBaseNodeSize)
		{
			Vector3I baseLowerCorner = mNodes[parent]->mRegion.getLowerCorner();
			int32_t childSize = (octreeConstructionMode == OctreeConstructionModes::BoundCells) ? mNodes[parent]->mRegion.getWidthInCells() / 2 : mNodes[parent]->mRegion.getWidthInVoxels() / 2;

			Vector3I baseUpperCorner;
			if(octreeConstructionMode == OctreeConstructionModes::BoundCells)
			{
				baseUpperCorner = baseLowerCorner + Vector3I(childSize, childSize, childSize);
			}
			else
			{
				baseUpperCorner = baseLowerCorner + Vector3I(childSize-1, childSize-1, childSize-1);
			}

			for(int z = 0; z < 2; z++)
			{
				for(int y = 0; y < 2; y++)
				{
					for(int x = 0; x < 2; x++)
					{
						Vector3I offset (x*childSize, y*childSize, z*childSize);
						Region childRegion(baseLowerCorner + offset, baseUpperCorner + offset);
						if(intersects(childRegion, regionToCover))
						{
							uint16_t octreeNode = createNode(childRegion, parent);
							mNodes[parent]->children[x][y][z] = octreeNode;
							buildOctreeNodeTree(octreeNode, regionToCover, octreeConstructionMode);
						}
					}
				}
			}
		}
	}

	template <typename VoxelType>
	void Octree<VoxelType>::clearWantedForRendering(uint16_t index)
	{
		mNodes[index]->mWantedForRendering = false;

		for(int iz = 0; iz < 2; iz++)
			{
				for(int iy = 0; iy < 2; iy++)
				{
					for(int ix = 0; ix < 2; ix++)
					{
						uint16_t child = mNodes[index]->children[ix][iy][iz];
						if(child)
						{
							clearWantedForRendering(child);
						}
					}
				}
			}
	}

	template <typename VoxelType>
	void Octree<VoxelType>::determineWantedForRendering(uint16_t index, const Vector3F& viewPosition, float lodThreshold)
	{
		OctreeNode<VoxelType>* node = mNodes[index];
		if(node->mLodLevel == 0)
		{
			node->mWantedForRendering = true;
		}
		else
		{
			Vector3F regionCentre = static_cast<Vector3F>(node->mRegion.getCentre());

			float distance = (viewPosition - regionCentre).length();

			Vector3I diagonal = node->mRegion.getUpperCorner() - node->mRegion.getLowerCorner();
			float diagonalLength = diagonal.length(); // A measure of our regions size

			float projectedSize = diagonalLength / distance;

			if((projectedSize > lodThreshold) || (node->mLodLevel > 2)) //subtree height check prevents building LODs for node near the root.
			{
				for(int iz = 0; iz < 2; iz++)
				{
					for(int iy = 0; iy < 2; iy++)
					{
						for(int ix = 0; ix < 2; ix++)
						{
							uint16_t child = node->children[ix][iy][iz];
							if(child)
							{
								determineWantedForRendering(child, viewPosition, lodThreshold);
							}
						}
					}
				}
			}
			else
			{
				node->mWantedForRendering = true;
			}
		}
	}

	template <typename VoxelType>
	void Octree<VoxelType>::determineWhetherToRender(uint16_t index)
	{
		OctreeNode<VoxelType>* node = mNodes[index];

		//At some point we should handle the issue that we might want to render but the mesh might not be ready.
		node->mRenderThisNode = node->mWantedForRendering;

		for(int iz = 0; iz < 2; iz++)
		{
			for(int iy = 0; iy < 2; iy++)
			{
				for(int ix = 0; ix < 2; ix++)
				{
					uint16_t child = node->children[ix][iy][iz];
					if(child)
					{
						determineWhetherToRender(child);
					}
				}
			}
		}
	}

	template <typename VoxelType>
	void Octree<VoxelType>::markAsModified(uint16_t index, int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority)
	{
		// Note - Can't this function just call the other version?

		OctreeNode<VoxelType>* node = mNodes[index];

		if(node->mRegion.containsPoint(x, y, z, -1)) //FIXME - Think if we really need this border.
		{
			//mIsMeshUpToDate = false;
			node->mDataLastModified = newTimeStamp;

			// Note: If DontUpdate was passed (an invalid choice) it will end up on the background thread.
			// Also we maintain mExtractOnMainThread if it was already set.
			node->mExtractOnMainThread = node->mExtractOnMainThread || (updatePriority == UpdatePriorities::Immediate);

			for(int iz = 0; iz < 2; iz++)
			{
				for(int iy = 0; iy < 2; iy++)
				{
					for(int ix = 0; ix < 2; ix++)
					{
						uint16_t child = node->children[ix][iy][iz];
						if(child)
						{
							markAsModified(child, x, y, z, newTimeStamp, updatePriority);
						}
					}
				}
			}
		}
	}

	template <typename VoxelType>
	void Octree<VoxelType>::markAsModified(uint16_t index, const Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority)
	{
		OctreeNode<VoxelType>* node = mNodes[index];

		if(intersects(node->mRegion, region))
		{
			//mIsMeshUpToDate = false;
			node->mDataLastModified = newTimeStamp;

			// Note: If DontUpdate was passed (an invalid choice) it will end up on the background thread.
			node->mExtractOnMainThread = (updatePriority == UpdatePriorities::Immediate);

			for(int iz = 0; iz < 2; iz++)
			{
				for(int iy = 0; iy < 2; iy++)
				{
					for(int ix = 0; ix < 2; ix++)
					{
						uint16_t child = node->children[ix][iy][iz];
						if(child)
						{
							markAsModified(child, region, newTimeStamp, updatePriority);
						}
					}
				}
			}
		}
	}
}