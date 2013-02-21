#include "SmoothTerrainVolume.h"

#include <algorithm>

using namespace std;
using namespace PolyVox;

SmoothTerrainVolume::SmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<MultiMaterial4>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundCells, baseNodeSize)
{
}

void SmoothTerrainVolume::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	Volume<typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType>::update(viewPosition, lodThreshold);

	if(mSmoothSurfaceExtractionTaskProcessor->hasAnyFinishedTasks())
	{
		SmoothSurfaceExtractionTask task = mSmoothSurfaceExtractionTaskProcessor->removeFirstFinishedTask();

		if(task.mOctreeNode->mLodLevel > 0)
		{
			recalculateMaterials(task.mSmoothMesh, static_cast<PolyVox::Vector3DFloat>(task.mOctreeNode->mRegion.getLowerCorner()), mVolData);
		}

		if(task.mSmoothMesh->getNoOfIndices() > 0) //But if the new mesh is empty we should still delete any old mesh?
		{
			task.mOctreeNode->buildGraphicsMesh(task.mSmoothMesh);
		}

		task.mOctreeNode->setMeshLastUpdated(getTime());
		task.mOctreeNode->mIsSceduledForUpdate = false;
	}
}

void SmoothTerrainVolume::updateMeshImpl(OctreeNode* octreeNode)
{
	SmoothSurfaceExtractionTask task(octreeNode, mVolData);

	mSmoothSurfaceExtractionTaskProcessor->addTask(task);

	/*PolyVox::Region lod0Region = octreeNode->mRegion;

	//Extract the surface
	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >* mesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >;

	generateSmoothMesh(lod0Region, octreeNode->mLodLevel, mesh);

	if(octreeNode->mLodLevel > 0)
	{
		recalculateMaterials(mesh, static_cast<PolyVox::Vector3DFloat>(lod0Region.getLowerCorner()), mVolData);
	}

	if(mesh->getNoOfIndices() > 0)
	{
		octreeNode->buildGraphicsMesh(mesh);
	}

	octreeNode->setMeshLastUpdated(getTime());
	octreeNode->mIsSceduledForUpdate = false;*/
}

/*void SmoothTerrainVolume::generateSmoothMesh(const PolyVox::Region& region, uint32_t lodLevel, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >* resultMesh)
{
	MultiMaterialMarchingCubesController<VoxelType> controller;

	if(lodLevel == 0)
	{
		//SurfaceMesh<PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > > mesh;
		PolyVox::MarchingCubesSurfaceExtractor< PolyVox::SimpleVolume<VoxelType>, MultiMaterialMarchingCubesController<VoxelType> > surfaceExtractor(mVolData, region, resultMesh, PolyVox::WrapModes::Border, VoxelType(0), controller);
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

		PolyVox::RawVolume<VoxelType> resampledVolume(lowRegion);

		resampleVolume(downSampleFactor, mVolData, highRegion, &resampledVolume, lowRegion);

		lowRegion.shrink(1, 1, 1);

		PolyVox::MarchingCubesSurfaceExtractor< PolyVox::RawVolume<VoxelType>, MultiMaterialMarchingCubesController<VoxelType> > surfaceExtractor(&resampledVolume, lowRegion, resultMesh, PolyVox::WrapModes::Border, VoxelType(0), controller);
		surfaceExtractor.execute();

		resultMesh->scaleVertices(downSampleFactor);
	}
}*/
