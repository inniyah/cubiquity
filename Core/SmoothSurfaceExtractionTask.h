#ifndef CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_
#define CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_

#include "MultiMaterial.h"
#include "OctreeNode.h"

#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SimpleVolume.h"

class SmoothSurfaceExtractionTask
{
public:
	SmoothSurfaceExtractionTask(OctreeNode* octreeNode, PolyVox::SimpleVolume< typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType >* volData);
	~SmoothSurfaceExtractionTask();

	void process(void);

	void generateSmoothMesh(const PolyVox::Region& region, uint32_t lodLevel, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType > >* resultMesh);

public:
	OctreeNode* mOctreeNode;
	PolyVox::SimpleVolume<typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType>* mVolData;
	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType> >* mSmoothMesh;
};

#endif //CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_
