#include "ColouredCubesVolume.h"

#include "ColouredCubicSurfaceExtractionTask.h"

using namespace PolyVox;

ColouredCubesVolume::ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<Colour>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundVoxels, baseNodeSize)
{
}

void ColouredCubesVolume::updateMeshImpl(OctreeNode* volReg)
{
	ColouredCubicSurfaceExtractionTask task(volReg, mVolData);
	task.process();

	if(task.mColouredCubicMesh->getNoOfIndices() > 0)
	{
		volReg->buildGraphicsMesh(task.mColouredCubicMesh/*, 0*/);
	}
}
