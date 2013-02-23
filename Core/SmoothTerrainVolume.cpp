#include "SmoothTerrainVolume.h"

#include "Clock.h"

#include <algorithm>

using namespace std;
using namespace PolyVox;

SmoothTerrainVolume::SmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<MultiMaterial4>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundCells, baseNodeSize)
{
}

void SmoothTerrainVolume::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	Volume<typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType>::update(viewPosition, lodThreshold);

	if(mSmoothSurfaceExtractionTaskProcessor->hasAnyFinishedTasks())
	{
		SmoothSurfaceExtractionTask task = mSmoothSurfaceExtractionTaskProcessor->removeFirstFinishedTask();

		if(task.mSmoothMesh->getNoOfIndices() > 0) //But if the new mesh is empty we should still delete any old mesh?
		{
			task.mOctreeNode->buildGraphicsMesh(task.mSmoothMesh);
		}

		task.mOctreeNode->setMeshLastUpdated(Clock::getTimestamp());
	}
}

void SmoothTerrainVolume::updateMeshImpl(OctreeNode* octreeNode)
{
	SmoothSurfaceExtractionTask task(octreeNode, mPolyVoxVolume);

	mSmoothSurfaceExtractionTaskProcessor->addTask(task);
}
