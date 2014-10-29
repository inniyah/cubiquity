#include "OctreeNode.h"
#include "Volume.h"
#include "MainThreadTaskProcessor.h"

namespace Cubiquity
{
	template <typename VoxelType>
	Octree<VoxelType>::Octree(Volume<VoxelType>* volume, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize)
		:mVolume(volume)
		,mRootNodeIndex(InvalidNodeIndex)
		,mBaseNodeSize(baseNodeSize)
		,mOctreeConstructionMode(octreeConstructionMode)
	{
		mRegionToCover = mVolume->getEnclosingRegion();
		if(mOctreeConstructionMode == OctreeConstructionModes::BoundVoxels)
		{
			mRegionToCover.shiftUpperCorner(1, 1, 1);
		}
		else if(mOctreeConstructionMode == OctreeConstructionModes::BoundCells)
		{
			mRegionToCover.shiftLowerCorner(-1, -1, -1);
			mRegionToCover.shiftUpperCorner(1, 1, 1);
		}

		POLYVOX_ASSERT(::PolyVox::isPowerOf2(mBaseNodeSize), "Node size must be a power of two");

		uint32_t largestVolumeDimension = (std::max)(mRegionToCover.getWidthInVoxels(), (std::max)(mRegionToCover.getHeightInVoxels(), mRegionToCover.getDepthInVoxels()));
		if(mOctreeConstructionMode == OctreeConstructionModes::BoundCells)
		{
			largestVolumeDimension--;
		}

		uint32_t octreeTargetSize = ::PolyVox::upperPowerOfTwo(largestVolumeDimension);

		uint8_t maxHeightOfTree = ::PolyVox::logBase2((octreeTargetSize) / mBaseNodeSize) + 1;

		uint32_t regionToCoverWidth = (mOctreeConstructionMode == OctreeConstructionModes::BoundCells) ? mRegionToCover.getWidthInCells() : mRegionToCover.getWidthInVoxels();
		uint32_t regionToCoverHeight = (mOctreeConstructionMode == OctreeConstructionModes::BoundCells) ? mRegionToCover.getHeightInCells() : mRegionToCover.getHeightInVoxels();
		uint32_t regionToCoverDepth = (mOctreeConstructionMode == OctreeConstructionModes::BoundCells) ? mRegionToCover.getDepthInCells() : mRegionToCover.getDepthInVoxels();

		uint32_t widthIncrease = octreeTargetSize - regionToCoverWidth;
		uint32_t heightIncrease = octreeTargetSize - regionToCoverHeight;
		uint32_t depthIncrease = octreeTargetSize - regionToCoverDepth;

		Region octreeRegion(mRegionToCover);
	
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

		mRootNodeIndex = createNode(octreeRegion, InvalidNodeIndex);
		mNodes[mRootNodeIndex]->mHeight = maxHeightOfTree - 1;

		buildOctreeNodeTree(mRootNodeIndex);
	}

	template <typename VoxelType>
	Octree<VoxelType>::~Octree()
	{
		for(uint32_t ct = 0; ct < mNodes.size(); ct++)
		{
			delete mNodes[ct];
		}
	}

	template <typename VoxelType>
	uint16_t Octree<VoxelType>::createNode(Region region, uint16_t parent)
	{
		OctreeNode< VoxelType >* node = new OctreeNode< VoxelType >(region, parent, this);

		if(parent != InvalidNodeIndex)
		{
			POLYVOX_ASSERT(mNodes[parent]->mHeight < 100, "Node height has gone below zero and wrapped around.");
			node->mHeight = mNodes[parent]->mHeight-1;
		}

		mNodes.push_back(node);
		POLYVOX_ASSERT(mNodes.size() < InvalidNodeIndex, "Too many octree nodes!");
		uint16_t index = mNodes.size() - 1;
		mNodes[index]->mSelf = index;
		return index;
	}

	template <typename VoxelType>
	void Octree<VoxelType>::update(const Vector3F& viewPosition, float lodThreshold)
	{
		acceptVisitor(ClearWantedForRenderingVisitor<VoxelType>());

		acceptVisitor(DetermineActiveNodesVisitor<VoxelType>(viewPosition, lodThreshold));

		acceptVisitor(DetermineWantedForRenderingVisitor<VoxelType>(viewPosition, lodThreshold));

		//determineWantedForRendering(mRootNodeIndex, viewPosition, lodThreshold);

		sceduleUpdateIfNeeded(mRootNodeIndex, viewPosition);


		// Make sure any surface extraction tasks which were scheduled on the main thread get processed before we determine what to render.
		gMainThreadTaskProcessor.processAllTasks(); //Doesn't really belong here

		// This will include tasks from both the background and main threads.
		while(!mFinishedSurfaceExtractionTasks.empty())
		{
			typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType* task;
			mFinishedSurfaceExtractionTasks.wait_and_pop(task);

			task->mOctreeNode->updateFromCompletedTask(task);

			if(task->mOctreeNode->mLastSurfaceExtractionTask == task)
			{
				task->mOctreeNode->mLastSurfaceExtractionTask = 0;
			}

			delete task;
		}

		acceptVisitor(DetermineWhetherToRenderVisitor<VoxelType>());

		//determineWhetherToRender(mRootNodeIndex);

		propagateTimestamps(mRootNodeIndex);
		propagateMeshTimestamps(mRootNodeIndex);
	}

	template <typename VoxelType>
	void Octree<VoxelType>::markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp, UpdatePriority updatePriority)
	{
		markAsModified(mRootNodeIndex, x, y, z, newTimeStamp, updatePriority);
	}

	template <typename VoxelType>
	void Octree<VoxelType>::markDataAsModified(const Region& region, Timestamp newTimeStamp, UpdatePriority updatePriority)
	{
		markAsModified(mRootNodeIndex, region, newTimeStamp, updatePriority);
	}

	template <typename VoxelType>
	void Octree<VoxelType>::buildOctreeNodeTree(uint16_t parent)
	{
		POLYVOX_ASSERT(mNodes[parent]->mRegion.getWidthInVoxels() == mNodes[parent]->mRegion.getHeightInVoxels(), "Region must be cubic");
		POLYVOX_ASSERT(mNodes[parent]->mRegion.getWidthInVoxels() == mNodes[parent]->mRegion.getDepthInVoxels(), "Region must be cubic");

		//We know that width/height/depth are all the same.
		uint32_t parentSize = static_cast<uint32_t>((mOctreeConstructionMode == OctreeConstructionModes::BoundCells) ? mNodes[parent]->mRegion.getWidthInCells() : mNodes[parent]->mRegion.getWidthInVoxels());

		if(parentSize > mBaseNodeSize)
		{
			Vector3I baseLowerCorner = mNodes[parent]->mRegion.getLowerCorner();
			int32_t childSize = (mOctreeConstructionMode == OctreeConstructionModes::BoundCells) ? mNodes[parent]->mRegion.getWidthInCells() / 2 : mNodes[parent]->mRegion.getWidthInVoxels() / 2;

			Vector3I baseUpperCorner;
			if(mOctreeConstructionMode == OctreeConstructionModes::BoundCells)
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
						if(intersects(childRegion, mRegionToCover))
						{
							uint16_t octreeNode = createNode(childRegion, parent);
							mNodes[parent]->children[x][y][z] = octreeNode;
							buildOctreeNodeTree(octreeNode);
						}
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

		Region dilatedRegion = node->mRegion;
		dilatedRegion.grow(1); //FIXME - Think if we really need this dilation?

		if(dilatedRegion.containsPoint(x, y, z))
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
						uint16_t childIndex = node->children[ix][iy][iz];
						if(childIndex != InvalidNodeIndex)
						{
							markAsModified(childIndex, x, y, z, newTimeStamp, updatePriority);
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
						uint16_t childIndex = node->children[ix][iy][iz];
						if(childIndex != InvalidNodeIndex)
						{
							markAsModified(childIndex, region, newTimeStamp, updatePriority);
						}
					}
				}
			}
		}
	}

	template <typename VoxelType>
	Timestamp Octree<VoxelType>::propagateTimestamps(uint16_t index)
	{
		OctreeNode<VoxelType>* node = mNodes[index];

		node->mStructureLastChangedRecursive = node->mStructureLastChanged;

		for (int iz = 0; iz < 2; iz++)
		{
			for (int iy = 0; iy < 2; iy++)
			{
				for (int ix = 0; ix < 2; ix++)
				{
					uint16_t childIndex = node->children[ix][iy][iz];
					if (childIndex != InvalidNodeIndex)
					{
						Timestamp subtreeTimestamp = propagateTimestamps(childIndex);
						node->mStructureLastChangedRecursive = (std::max)(node->mStructureLastChangedRecursive, subtreeTimestamp);
					}
				}
			}
		}

		return node->mStructureLastChangedRecursive;
	}

	template <typename VoxelType>
	Timestamp Octree<VoxelType>::propagateMeshTimestamps(uint16_t index)
	{
		OctreeNode<VoxelType>* node = mNodes[index];

		node->mMeshLastChangedRecursive = node->mMeshLastChanged;

		for (int iz = 0; iz < 2; iz++)
		{
			for (int iy = 0; iy < 2; iy++)
			{
				for (int ix = 0; ix < 2; ix++)
				{
					uint16_t childIndex = node->children[ix][iy][iz];
					if (childIndex != InvalidNodeIndex)
					{
						Timestamp subtreeTimestamp = propagateMeshTimestamps(childIndex);
						node->mMeshLastChangedRecursive = (std::max)(node->mMeshLastChangedRecursive, subtreeTimestamp);
					}
				}
			}
		}

		return node->mMeshLastChangedRecursive;
	}

	template <typename VoxelType>
	void Octree<VoxelType>::sceduleUpdateIfNeeded(uint16_t index, const Vector3F& viewPosition)
	{
		OctreeNode<VoxelType>* node = mNodes[index];

		if ((node->isMeshUpToDate() == false) && (node->isSceduledForUpdate() == false) && ((node->mLastSurfaceExtractionTask == 0) || (node->mLastSurfaceExtractionTask->mProcessingStartedTimestamp < Clock::getTimestamp())) && (node->mWantedForRendering))
		{
			node->mLastSceduledForUpdate = Clock::getTimestamp();

			node->mLastSurfaceExtractionTask = new typename VoxelTraits<VoxelType>::SurfaceExtractionTaskType(node, mVolume->mPolyVoxVolume);

			// If the node was rendered last frame then this update is probably the result of an editing operation, rather than
			// the node only just becoming visible. For editing operations it is important to process them immediatly so that we
			// don't see temporary cracks in the mesh as different parts up updated at different times.
			//if(node->mExtractOnMainThread) //This flag should still be set from last frame.
			{
				// We're going to process immediatly, but the completed task will still get queued in the finished
				// queue, and we want to make sure it's the first out. So we still set a priority and make it high.
				node->mLastSurfaceExtractionTask->mPriority = (std::numeric_limits<uint32_t>::max)();
				gMainThreadTaskProcessor.addTask(node->mLastSurfaceExtractionTask);
			}
			/*else
			{
			// Note: tasks get sorted by their distance from the camera at the time they are added. If we
			// want to account for the camera moving then we would have to sort the task queue each frame.
			Vector3F regionCentre = static_cast<Vector3F>(node->mRegion.getCentre());
			float distance = (viewPosition - regionCentre).length(); //We don't use distance squared to keep the values smaller
			node->mLastSurfaceExtractionTask->mPriority = (std::numeric_limits<uint32_t>::max)() - static_cast<uint32_t>(distance);
			gBackgroundTaskProcessor.addTask(node->mLastSurfaceExtractionTask);
			}*/

			// Clear this flag otherwise this node will always be done on the main thread.
			node->mExtractOnMainThread = false;
		}

		for (int iz = 0; iz < 2; iz++)
		{
			for (int iy = 0; iy < 2; iy++)
			{
				for (int ix = 0; ix < 2; ix++)
				{
					uint16_t childIndex = node->children[ix][iy][iz];
					if (childIndex != InvalidNodeIndex)
					{
						sceduleUpdateIfNeeded(childIndex, viewPosition);
					}
				}
			}
		}
	}

	template <typename VoxelType>
	bool Octree<VoxelType>::determineWhetherToRender(uint16_t index)
	{
		OctreeNode<VoxelType>* node = mNodes[index];

		node->mRenderThisNode = false; // node->isMeshUpToDate();

		bool renderAllChildren = true;

		for(int iz = 0; iz < 2; iz++)
		{
			for(int iy = 0; iy < 2; iy++)
			{
				for(int ix = 0; ix < 2; ix++)
				{
					uint16_t childIndex = node->children[ix][iy][iz];
					if(childIndex != InvalidNodeIndex)
					{
						OctreeNode<VoxelType>* childNode = mNodes[childIndex];
						if (childNode->isActive())
						{
							renderAllChildren = renderAllChildren && determineWhetherToRender(childIndex);
						}
						else
						{
							renderAllChildren = false;
						}
					}
				}
			}
		}

		if (renderAllChildren == false)
		{
			node->mRenderThisNode = true;
		}

		return node->mRenderThisNode;
	}

	/*template <typename VoxelType>
	void Octree<VoxelType>::determineWantedForRendering(uint16_t index, const Vector3F& viewPosition, float lodThreshold)
	{
		OctreeNode<VoxelType>* octreeNode = mNodes[index];

		if(octreeNode->mHeight == 0)
		{
			octreeNode->mWantedForRendering = true;
		}
		else
		{
			Vector3F regionCentre = static_cast<Vector3F>(octreeNode->mRegion.getCentre());

			float distance = (viewPosition - regionCentre).length();

			Vector3I diagonal = octreeNode->mRegion.getUpperCorner() - octreeNode->mRegion.getLowerCorner();
			float diagonalLength = diagonal.length(); // A measure of our regions size

			float projectedSize = diagonalLength / distance;

			bool processChildren = ((projectedSize > lodThreshold) || (octreeNode->mHeight > 2)); //subtree height check prevents building LODs for node near the root.

			if(processChildren)
			{
				for(int z = 0; z < 2; z++)
				{
					for(int y = 0; y < 2; y++)
					{
						for(int x = 0; x < 2; x++)
						{
							uint16_t childIndex = octreeNode->children[x][y][z];

							//If the node doesn't exist we create it.
							if(childIndex == InvalidNodeIndex)
							{
								Vector3I baseLowerCorner = octreeNode->mRegion.getLowerCorner();
								int32_t childSize = (mOctreeConstructionMode == OctreeConstructionModes::BoundCells) ? octreeNode->mRegion.getWidthInCells() / 2 : octreeNode->mRegion.getWidthInVoxels() / 2;

								Vector3I baseUpperCorner;
								if(mOctreeConstructionMode == OctreeConstructionModes::BoundCells)
								{
									baseUpperCorner = baseLowerCorner + Vector3I(childSize, childSize, childSize);
								}
								else
								{
									baseUpperCorner = baseLowerCorner + Vector3I(childSize-1, childSize-1, childSize-1);
								}

								Vector3I offset (x*childSize, y*childSize, z*childSize);
								Region childRegion(baseLowerCorner + offset, baseUpperCorner + offset);
								if(intersects(childRegion, mRegionToCover))
								{
									childIndex = createNode(childRegion, index);
									octreeNode->children[x][y][z] = childIndex;
								}
							}

							if(childIndex != InvalidNodeIndex)
							{
								determineWantedForRendering(childIndex, viewPosition, lodThreshold);
							}
						}
					}
				}
			}
			else
			{
				octreeNode->mWantedForRendering = true;
			}
		}
	}*/

	template <typename VoxelType>
	template<typename VisitorType>
	void Octree<VoxelType>::visitNode(uint16_t index, VisitorType visitor)
	{
		OctreeNode<VoxelType>* node = mNodes[index];

		bool processChildren = visitor(node);

		if(processChildren)
		{
			for(int iz = 0; iz < 2; iz++)
			{
				for(int iy = 0; iy < 2; iy++)
				{
					for(int ix = 0; ix < 2; ix++)
					{
						uint16_t childIndex = node->children[ix][iy][iz];
						if(childIndex != InvalidNodeIndex)
						{
							visitNode(childIndex, visitor);
						}
					}
				}
			}
		}
	}
}