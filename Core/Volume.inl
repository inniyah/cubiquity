#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Raycast.h"
#include "PolyVoxCore/VolumeResampler.h"

#include "PolyVoxCore/Impl/Utility.h" //Should we include from Impl?

#include "MultiMaterialMarchingCubesController.h"
#include "ColouredCubesIsQuadNeeded.h"

#include "Raycasting.h"

#include "CubiquityUtility.h"

template <typename VoxelType>
Volume<VoxelType>::Volume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize)
	:mVolData(0)
	,mRootOctreeNode(0)
	,mBaseNodeSize(baseNodeSize)
	,mTime(0)
{
	PolyVox::Region volumeRegion(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);

	POLYVOX_ASSERT(volumeRegion.getWidthInVoxels() > 0, "All volume dimensions must be greater than zero");
	POLYVOX_ASSERT(volumeRegion.getHeightInVoxels() > 0, "All volume dimensions must be greater than zero");
	POLYVOX_ASSERT(volumeRegion.getDepthInVoxels() > 0, "All volume dimensions must be greater than zero");
	
	mVolData = new PolyVox::RawVolume<VoxelType>(volumeRegion);

	PolyVox::Region regionToCover(mVolData->getEnclosingRegion());
	if(octreeConstructionMode == OctreeConstructionModes::BoundCells)
	{
		regionToCover.shiftLowerCorner(-1, -1, -1);
		regionToCover.shiftUpperCorner(1, 1, 1);
	}

	GP_ASSERT(PolyVox::isPowerOf2(mBaseNodeSize));

	uint32_t largestVolumeDimension = std::max(regionToCover.getWidthInVoxels(), std::max(regionToCover.getHeightInVoxels(), regionToCover.getDepthInVoxels()));
	if(octreeConstructionMode == OctreeConstructionModes::BoundCells)
	{
		largestVolumeDimension--;
	}

	uint32_t octreeTargetSize = PolyVox::upperPowerOfTwo(largestVolumeDimension);

	uint8_t noOfLodLevels = logBase2((octreeTargetSize) / mBaseNodeSize) + 1;

	uint32_t regionToCoverWidth = (octreeConstructionMode == OctreeConstructionModes::BoundCells) ? regionToCover.getWidthInCells() : regionToCover.getWidthInVoxels();
	uint32_t regionToCoverHeight = (octreeConstructionMode == OctreeConstructionModes::BoundCells) ? regionToCover.getHeightInCells() : regionToCover.getHeightInVoxels();
	uint32_t regionToCoverDepth = (octreeConstructionMode == OctreeConstructionModes::BoundCells) ? regionToCover.getDepthInCells() : regionToCover.getDepthInVoxels();

	uint32_t widthIncrease = octreeTargetSize - regionToCoverWidth;
	uint32_t heightIncrease = octreeTargetSize - regionToCoverHeight;
	uint32_t depthIncrease = octreeTargetSize - regionToCoverDepth;

	PolyVox::Region octreeRegion(regionToCover);
	
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

	mRootOctreeNode = new OctreeNode(octreeRegion, 0);
	mRootOctreeNode->mLodLevel = noOfLodLevels - 1;

	buildOctreeNodeTree(mRootOctreeNode, regionToCover, octreeConstructionMode);
}

template <typename VoxelType>
void Volume<VoxelType>::buildOctreeNodeTree(OctreeNode* parent, const PolyVox::Region& regionToCover, OctreeConstructionMode octreeConstructionMode)
{
	POLYVOX_ASSERT(parent->mRegion.getWidthInVoxels() == parent->mRegion.getHeightInVoxels(), "Region must be cubic");
	POLYVOX_ASSERT(parent->mRegion.getWidthInVoxels() == parent->mRegion.getDepthInVoxels(), "Region must be cubic");

	//We know that width/height/depth are all the same.
	int32_t parentSize = (octreeConstructionMode == OctreeConstructionMode::BoundCells) ? parent->mRegion.getWidthInCells() : parent->mRegion.getWidthInVoxels();

	if(parentSize > mBaseNodeSize)
	{
		PolyVox::Vector3DInt32 baseLowerCorner = parent->mRegion.getLowerCorner();
		int32_t childSize = (octreeConstructionMode == OctreeConstructionMode::BoundCells) ? parent->mRegion.getWidthInCells() / 2 : parent->mRegion.getWidthInVoxels() / 2;

		PolyVox::Vector3DInt32 baseUpperCorner;
		if(octreeConstructionMode == OctreeConstructionMode::BoundCells)
		{
			baseUpperCorner = baseLowerCorner + PolyVox::Vector3DInt32(childSize, childSize, childSize);
		}
		else
		{
			baseUpperCorner = baseLowerCorner + PolyVox::Vector3DInt32(childSize-1, childSize-1, childSize-1);
		}

		for(int z = 0; z < 2; z++)
		{
			for(int y = 0; y < 2; y++)
			{
				for(int x = 0; x < 2; x++)
				{
					PolyVox::Vector3DInt32 offset (x*childSize, y*childSize, z*childSize);
					PolyVox::Region childRegion(baseLowerCorner + offset, baseUpperCorner + offset);
					if(intersects(childRegion, regionToCover))
					{
						OctreeNode* volReg = new OctreeNode(childRegion, parent);
						parent->children[x][y][z] = volReg;
						buildOctreeNodeTree(volReg, regionToCover, octreeConstructionMode);
					}
				}
			}
		}
	}
}

template <typename VoxelType>
Volume<VoxelType>::~Volume()
{
}

template <typename VoxelType>
VoxelType Volume<VoxelType>::getVoxelAt(int x, int y, int z)
{
	return mVolData->getVoxelAt(x, y, z);
}

template <typename VoxelType>
void Volume<VoxelType>::setVoxelAt(int x, int y, int z, VoxelType value)
{
	mVolData->setVoxelAt(x, y, z, value);
	mRootOctreeNode->markDataAsModified(x, y, z, getTime());
}

template <typename VoxelType>
void Volume<VoxelType>::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	mRootOctreeNode->clearWantedForRendering();
	mRootOctreeNode->determineWantedForRendering(viewPosition, lodThreshold);

	updateMesh(mRootOctreeNode);
}

template <typename VoxelType>
void Volume<VoxelType>::updateMesh(OctreeNode* volReg)
{
	if((volReg->isMeshUpToDate() == false) && (volReg->mWantedForRendering))
	{
		updateMeshImpl(volReg);

		volReg->setMeshLastUpdated(getTime());
	}

	for(int z = 0; z < 2; z++)
	{
		for(int y = 0; y < 2; y++)
		{
			for(int x = 0; x < 2; x++)
			{
				OctreeNode* child = volReg->children[x][y][z];
				if(child)
				{
					updateMesh(child);
				}
			}
		}
	}
}
