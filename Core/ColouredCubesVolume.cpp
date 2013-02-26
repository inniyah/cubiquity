#include "ColouredCubesVolume.h"

#include "Clock.h"
#include "Colour.h"
#include "ColouredCubicSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

using namespace PolyVox;

ColouredCubesVolume::ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<Colour>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundVoxels, baseNodeSize)
{
}

void ColouredCubesVolume::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	Volume<Colour>::update(viewPosition, lodThreshold);

	if(gMainThreadTaskProcessor.hasAnyFinishedTasks())
	{
		ColouredCubicSurfaceExtractionTask* task = dynamic_cast<ColouredCubicSurfaceExtractionTask*>(gMainThreadTaskProcessor.removeFirstFinishedTask());

		if(task->mColouredCubicMesh->getNoOfIndices() > 0) //But if the new mesh is empty we should still delete any old mesh?
		{
			task->mOctreeNode->mPolyVoxMesh = task->mColouredCubicMesh;
		}

		task->mOctreeNode->setMeshLastUpdated(Clock::getTimestamp());
	}
}
