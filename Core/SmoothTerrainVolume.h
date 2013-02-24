#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "SmoothSurfaceExtractionTask.h"
#include "TaskProcessor.h"
#include "MultiMaterial.h"
#include "Volume.h"

#include "PolyVoxCore/MaterialDensityPair.h"

class SmoothTerrainVolume : public Volume<MultiMaterial4>
{

public:
	SmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);

	virtual void update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	void updateMeshImpl(OctreeNode< typename VoxelTraits<VoxelType>::VertexType >* octreeNode);

	//This task processor must be created and set by out engine-specific derived class.
	TaskProcessor<SmoothSurfaceExtractionTask>* mSmoothSurfaceExtractionTaskProcessor;
};

#endif //SMOOTHTERRAINVOLUME_H_
