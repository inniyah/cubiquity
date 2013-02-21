#ifndef CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_
#define CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_

#include "MultiMaterial.h"
#include "OctreeNode.h"

#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SimpleVolume.h"

class SmoothSurfaceExtractionTask
{
public:
	SmoothSurfaceExtractionTask(OctreeNode* octreeNode, PolyVox::SimpleVolume<Colour>* volData);
	~SmoothSurfaceExtractionTask();

	void process(void);

public:
	OctreeNode* mOctreeNode;
	PolyVox::SimpleVolume<Colour>* mVolData;
	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType> >* mSmoothMesh;
};

#endif //CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_
