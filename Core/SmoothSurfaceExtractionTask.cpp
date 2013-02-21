#include "SmoothSurfaceExtractionTask.h"

#include "ColouredCubesIsQuadNeeded.h"

#include "PolyVoxCore/RawVolume.h"

using namespace PolyVox;

SmoothSurfaceExtractionTask::SmoothSurfaceExtractionTask(OctreeNode* octreeNode, PolyVox::SimpleVolume<Colour>* volData)
	:mOctreeNode(octreeNode)
	,mVolData(volData)
	,mSmoothMesh(0)
{
}

SmoothSurfaceExtractionTask::~SmoothSurfaceExtractionTask()
{
	//Should delete mesh here?
}

void SmoothSurfaceExtractionTask::process(void)
{
}