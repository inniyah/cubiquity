#include "ColouredCubesVolume.h"

#include "Clock.h"
#include "Colour.h"
#include "ColouredCubicSurfaceExtractionTask.h"
#include "TaskProcessor.h"

using namespace PolyVox;

ColouredCubesVolume::ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<Colour>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundVoxels, baseNodeSize)
{
}

void ColouredCubesVolume::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	Volume<Colour>::update(viewPosition, lodThreshold);

	if(mSurfaceExtractionTaskProcessor->hasAnyFinishedTasks())
	{
		ColouredCubicSurfaceExtractionTask task = mSurfaceExtractionTaskProcessor->removeFirstFinishedTask();

		if(task.mColouredCubicMesh->getNoOfIndices() > 0) //But if the new mesh is empty we should still delete any old mesh?
		{
			task.mOctreeNode->mPolyVoxMesh = task.mColouredCubicMesh;
		}

		task.mOctreeNode->setMeshLastUpdated(Clock::getTimestamp());
	}
}

void ColouredCubesVolume::updateMeshImpl(OctreeNode< typename VoxelTraits<VoxelType>::VertexType >* octreeNode)
{
	ColouredCubicSurfaceExtractionTask task(octreeNode, mPolyVoxVolume);

	mSurfaceExtractionTaskProcessor->addTask(task);
}
