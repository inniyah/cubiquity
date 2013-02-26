#include "SmoothTerrainVolume.h"

#include "Clock.h"
#include "MultiMaterial.h"
#include "SmoothSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

#include <algorithm>

using namespace std;
using namespace PolyVox;

SmoothTerrainVolume::SmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<MultiMaterial>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundCells, baseNodeSize)
{
}

void SmoothTerrainVolume::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	Volume<typename MultiMaterialMarchingCubesController::MaterialType>::update(viewPosition, lodThreshold);

	if(gMainThreadTaskProcessor.hasAnyFinishedTasks())
	{
		SmoothSurfaceExtractionTask* task = dynamic_cast<SmoothSurfaceExtractionTask*>(gMainThreadTaskProcessor.removeFirstFinishedTask());

		if(task->mSmoothMesh->getNoOfIndices() > 0) //But if the new mesh is empty we should still delete any old mesh?
		{
			task->mOctreeNode->mPolyVoxMesh = task->mSmoothMesh;
		}

		task->mOctreeNode->setMeshLastUpdated(Clock::getTimestamp());
	}
}
