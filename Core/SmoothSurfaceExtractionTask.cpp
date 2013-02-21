#include "SmoothSurfaceExtractionTask.h"

#include "ColouredCubesIsQuadNeeded.h"

#include "SmoothTerrainVolume.h" //Temporary

#include "PolyVoxCore/RawVolume.h"

using namespace PolyVox;

SmoothSurfaceExtractionTask::SmoothSurfaceExtractionTask(OctreeNode* octreeNode, PolyVox::SimpleVolume<typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType>* volData)
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
	//Extract the surface
	mSmoothMesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType > >;

	generateSmoothMesh(mOctreeNode->mRegion, mOctreeNode->mLodLevel, mSmoothMesh);
}

void SmoothSurfaceExtractionTask::generateSmoothMesh(const PolyVox::Region& region, uint32_t lodLevel, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType > >* resultMesh)
{
	MultiMaterialMarchingCubesController<MultiMaterial4> controller;

	if(lodLevel == 0)
	{
		//SurfaceMesh<PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > > mesh;
		PolyVox::MarchingCubesSurfaceExtractor< PolyVox::SimpleVolume<MultiMaterial4>, MultiMaterialMarchingCubesController<MultiMaterial4> > surfaceExtractor(mVolData, region, resultMesh, PolyVox::WrapModes::Border, MultiMaterial4(0), controller);
		surfaceExtractor.execute();
	}
	else
	{
		uint32_t downSampleFactor = 0x0001 << lodLevel;

		int crackHidingFactor = 5; //This should probably be configurable?
		controller.setThreshold(controller.getThreshold() + (downSampleFactor * crackHidingFactor));

		PolyVox::Region highRegion = region;
		highRegion.grow(downSampleFactor, downSampleFactor, downSampleFactor);

		PolyVox::Region lowRegion = highRegion;
		PolyVox::Vector3DInt32 lowerCorner = lowRegion.getLowerCorner();
		PolyVox::Vector3DInt32 upperCorner = lowRegion.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(downSampleFactor);
		upperCorner = upperCorner + lowerCorner;
		lowRegion.setUpperCorner(upperCorner);

		PolyVox::RawVolume<MultiMaterial4> resampledVolume(lowRegion);

		resampleVolume(downSampleFactor, mVolData, highRegion, &resampledVolume, lowRegion);

		lowRegion.shrink(1, 1, 1);

		PolyVox::MarchingCubesSurfaceExtractor< PolyVox::RawVolume<MultiMaterial4>, MultiMaterialMarchingCubesController<MultiMaterial4> > surfaceExtractor(&resampledVolume, lowRegion, resultMesh, PolyVox::WrapModes::Border, MultiMaterial4(0), controller);
		surfaceExtractor.execute();

		resultMesh->scaleVertices(downSampleFactor);
	}
}