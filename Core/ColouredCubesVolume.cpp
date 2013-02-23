#include "ColouredCubesVolume.h"

#include "Clock.h"
#include "ColouredCubicSurfaceExtractionTask.h"

using namespace PolyVox;

ColouredCubesVolume::ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<Colour>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundVoxels, baseNodeSize)
	,mColouredCubicSurfaceExtractionTaskProcessor(0)
{
}

void ColouredCubesVolume::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	Volume<Colour>::update(viewPosition, lodThreshold);

	if(mColouredCubicSurfaceExtractionTaskProcessor->hasAnyFinishedTasks())
	{
		ColouredCubicSurfaceExtractionTask task = mColouredCubicSurfaceExtractionTaskProcessor->removeFirstFinishedTask();

		if(task.mColouredCubicMesh->getNoOfIndices() > 0) //But if the new mesh is empty we should still delete any old mesh?
		{
			task.mOctreeNode->buildGraphicsMesh(task.mColouredCubicMesh);
		}

		task.mOctreeNode->setMeshLastUpdated(Clock::getTimestamp());
	}
}

void ColouredCubesVolume::updateMeshImpl(OctreeNode< VoxelTraits<VoxelType> >* octreeNode)
{
	ColouredCubicSurfaceExtractionTask task(octreeNode, mPolyVoxVolume);

	mColouredCubicSurfaceExtractionTaskProcessor->addTask(task);
}
