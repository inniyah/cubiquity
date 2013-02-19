#include "ColouredCubesVolume.h"

#include "CubicSurfaceExtractionTask.h"

using namespace PolyVox;

ColouredCubesVolume::ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<Colour>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundVoxels, baseNodeSize)
{
}

void ColouredCubesVolume::updateMeshImpl(OctreeNode* volReg)
{
	PolyVox::Region lod0Region = volReg->mRegion;

	//Extract the surface
	PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >* colouredCubicMesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >;

	uint32_t downScaleFactor = 0x0001 << volReg->mLodLevel;

	generateCubicMesh(lod0Region, downScaleFactor, colouredCubicMesh);

	if(colouredCubicMesh->getNoOfIndices() > 0)
	{
		volReg->buildGraphicsMesh(colouredCubicMesh/*, 0*/);
	}
}

void ColouredCubesVolume::generateCubicMesh(const PolyVox::Region& region, uint32_t downSampleFactor, PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >* resultMesh)
{
	ColouredCubesIsQuadNeeded<VoxelType> isQuadNeeded;

	if(downSampleFactor == 1) 
	{
		PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> > colouredCubicMesh;
		CubicSurfaceExtractionTask< PolyVox::SimpleVolume<VoxelType>, ColouredCubesIsQuadNeeded<VoxelType> > surfaceExtractionTask(mVolData, region, resultMesh, PolyVox::WrapModes::Border, VoxelType(0), true, isQuadNeeded);
		surfaceExtractionTask.process();
	}
	else if(downSampleFactor == 2)
	{
		
		PolyVox::Region srcRegion = region;

		srcRegion.grow(2);

		PolyVox::Vector3DInt32 lowerCorner = srcRegion.getLowerCorner();
		PolyVox::Vector3DInt32 upperCorner = srcRegion.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(downSampleFactor);
		upperCorner = upperCorner + lowerCorner;

		PolyVox::Region dstRegion(lowerCorner, upperCorner);

		PolyVox::RawVolume<VoxelType> resampledVolume(dstRegion);
		rescaleCubicVolume(mVolData, srcRegion, &resampledVolume, dstRegion);

		dstRegion.shrink(1);
		
		//dstRegion.shiftLowerCorner(-1, -1, -1);

		PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> > colouredCubicMesh;
		CubicSurfaceExtractionTask< PolyVox::RawVolume<VoxelType>, ColouredCubesIsQuadNeeded<VoxelType> > surfaceExtractionTask(&resampledVolume, dstRegion, resultMesh, PolyVox::WrapModes::Border, VoxelType(0), true, isQuadNeeded);
		surfaceExtractionTask.process();

		resultMesh->scaleVertices(downSampleFactor);
		resultMesh->translateVertices(Vector3DFloat(0.5f, 0.5f, 0.5f));
	}
	else if(downSampleFactor == 4)
	{
		PolyVox::Region srcRegion = region;

		srcRegion.grow(4);

		PolyVox::Vector3DInt32 lowerCorner = srcRegion.getLowerCorner();
		PolyVox::Vector3DInt32 upperCorner = srcRegion.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(2);
		upperCorner = upperCorner + lowerCorner;

		PolyVox::Region dstRegion(lowerCorner, upperCorner);

		PolyVox::RawVolume<VoxelType> resampledVolume(dstRegion);
		rescaleCubicVolume(mVolData, srcRegion, &resampledVolume, dstRegion);



		lowerCorner = dstRegion.getLowerCorner();
		upperCorner = dstRegion.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(2);
		upperCorner = upperCorner + lowerCorner;

		PolyVox::Region dstRegion2(lowerCorner, upperCorner);

		PolyVox::RawVolume<VoxelType> resampledVolume2(dstRegion2);
		rescaleCubicVolume(&resampledVolume, dstRegion, &resampledVolume2, dstRegion2);

		dstRegion2.shrink(1);

		//dstRegion.shiftLowerCorner(-1, -1, -1);

		PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> > colouredCubicMesh;
		CubicSurfaceExtractionTask< PolyVox::RawVolume<VoxelType>, ColouredCubesIsQuadNeeded<VoxelType> > surfaceExtractionTask(&resampledVolume2, dstRegion2, resultMesh, PolyVox::WrapModes::Border, VoxelType(0), true, isQuadNeeded);
		surfaceExtractionTask.process();

		resultMesh->scaleVertices(downSampleFactor);
		resultMesh->translateVertices(Vector3DFloat(1.5f, 1.5f, 1.5f));
	}
}