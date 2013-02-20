#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "ColouredCubicSurfaceExtractionTask.h"
#include "TaskProcessor.h"
#include "Volume.h"

#include "Colour.h"
#include "PolyVoxCore/Material.h"

class ColouredCubesVolume : public Volume<Colour>
{
public:
	ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);

	virtual void update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	void updateMeshImpl(OctreeNode* volReg);

	TaskProcessor<ColouredCubicSurfaceExtractionTask> mPendingCubicSurfaceExtractionTasks;
	TaskProcessor<ColouredCubicSurfaceExtractionTask> mFinishedCubicSurfaceExtractionTasks;
};

#endif //COLOUREDCUBESVOLUME_H_
