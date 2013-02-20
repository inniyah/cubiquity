#include "ColouredCubesVolume.h"

#include "ColouredCubicSurfaceExtractionTask.h"

using namespace PolyVox;

ColouredCubesVolume::ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<Colour>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundVoxels, baseNodeSize)
{
}

void ColouredCubesVolume::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	Volume<Colour>::update(viewPosition, lodThreshold);

	if(mPendingCubicSurfaceExtractionTasks.size())
	{
		ColouredCubicSurfaceExtractionTask task = mPendingCubicSurfaceExtractionTasks.front();
		mPendingCubicSurfaceExtractionTasks.pop_front();

		task.process();

		if(task.mColouredCubicMesh->getNoOfIndices() > 0)
		{
			task.mOctreeNode->buildGraphicsMesh(task.mColouredCubicMesh);
		}

		task.mOctreeNode->setMeshLastUpdated(getTime());
		task.mOctreeNode->mIsSceduledForUpdate = false;
	}
}

void ColouredCubesVolume::updateMeshImpl(OctreeNode* volReg)
{
	ColouredCubicSurfaceExtractionTask task(volReg, mVolData);

	mPendingCubicSurfaceExtractionTasks.push_back(task);
	/*task.process();

	if(task.mColouredCubicMesh->getNoOfIndices() > 0)
	{
		volReg->buildGraphicsMesh(task.mColouredCubicMesh);
	}

	volReg->setMeshLastUpdated(getTime());
	volReg->mIsSceduledForUpdate = false;*/
}
