#include "ColouredCubicSurfaceExtractionTask.h"

#include "Colour.h"
#include "ColouredCubesIsQuadNeeded.h"
#include "OctreeNode.h"

#include "PolyVoxCore/RawVolume.h"

using namespace PolyVox;

ColouredCubicSurfaceExtractionTask::ColouredCubicSurfaceExtractionTask(OctreeNode< typename VoxelTraits<Colour>::VertexType >* octreeNode, PolyVox::SimpleVolume<Colour>* polyVoxVolume)
	:mOctreeNode(octreeNode)
	,mPolyVoxVolume(polyVoxVolume)
	,mColouredCubicMesh(0)
{
}

ColouredCubicSurfaceExtractionTask::~ColouredCubicSurfaceExtractionTask()
{
	//Should delete mesh here?
}

void ColouredCubicSurfaceExtractionTask::process(void)
{
	PolyVox::Region lod0Region = mOctreeNode->mRegion;

	//Extract the surface
	mColouredCubicMesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterial<Colour> >;

	uint32_t downScaleFactor = 0x0001 << mOctreeNode->mLodLevel;

	ColouredCubesIsQuadNeeded<Colour> isQuadNeeded;

	if(downScaleFactor == 1) 
	{
		PolyVox::CubicSurfaceExtractor< PolyVox::SimpleVolume<Colour>, ColouredCubesIsQuadNeeded<Colour> > surfaceExtractor(mPolyVoxVolume, mOctreeNode->mRegion, mColouredCubicMesh, PolyVox::WrapModes::Border, Colour(0), true, isQuadNeeded);
		surfaceExtractor.execute();
	}
	else if(downScaleFactor == 2)
	{
		
		PolyVox::Region srcRegion = mOctreeNode->mRegion;

		srcRegion.grow(2);

		PolyVox::Vector3DInt32 lowerCorner = srcRegion.getLowerCorner();
		PolyVox::Vector3DInt32 upperCorner = srcRegion.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(downScaleFactor);
		upperCorner = upperCorner + lowerCorner;

		PolyVox::Region dstRegion(lowerCorner, upperCorner);

		PolyVox::RawVolume<Colour> resampledVolume(dstRegion);
		rescaleCubicVolume(mPolyVoxVolume, srcRegion, &resampledVolume, dstRegion);

		dstRegion.shrink(1);
		
		//dstRegion.shiftLowerCorner(-1, -1, -1);

		PolyVox::CubicSurfaceExtractor< PolyVox::RawVolume<Colour>, ColouredCubesIsQuadNeeded<Colour> > surfaceExtractor(&resampledVolume, dstRegion, mColouredCubicMesh, PolyVox::WrapModes::Border, Colour(0), true, isQuadNeeded);
		surfaceExtractor.execute();

		mColouredCubicMesh->scaleVertices(static_cast<float>(downScaleFactor));
		mColouredCubicMesh->translateVertices(Vector3DFloat(0.5f, 0.5f, 0.5f));
	}
	else if(downScaleFactor == 4)
	{
		PolyVox::Region srcRegion = mOctreeNode->mRegion;

		srcRegion.grow(4);

		PolyVox::Vector3DInt32 lowerCorner = srcRegion.getLowerCorner();
		PolyVox::Vector3DInt32 upperCorner = srcRegion.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(2);
		upperCorner = upperCorner + lowerCorner;

		PolyVox::Region dstRegion(lowerCorner, upperCorner);

		PolyVox::RawVolume<Colour> resampledVolume(dstRegion);
		rescaleCubicVolume(mPolyVoxVolume, srcRegion, &resampledVolume, dstRegion);



		lowerCorner = dstRegion.getLowerCorner();
		upperCorner = dstRegion.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(2);
		upperCorner = upperCorner + lowerCorner;

		PolyVox::Region dstRegion2(lowerCorner, upperCorner);

		PolyVox::RawVolume<Colour> resampledVolume2(dstRegion2);
		rescaleCubicVolume(&resampledVolume, dstRegion, &resampledVolume2, dstRegion2);

		dstRegion2.shrink(1);

		//dstRegion.shiftLowerCorner(-1, -1, -1);

		PolyVox::CubicSurfaceExtractor< PolyVox::RawVolume<Colour>, ColouredCubesIsQuadNeeded<Colour> > surfaceExtractor(&resampledVolume2, dstRegion2, mColouredCubicMesh, PolyVox::WrapModes::Border, Colour(0), true, isQuadNeeded);
		surfaceExtractor.execute();

		mColouredCubicMesh->scaleVertices(static_cast<float>(downScaleFactor));
		mColouredCubicMesh->translateVertices(Vector3DFloat(1.5f, 1.5f, 1.5f));
	}
}