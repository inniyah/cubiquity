#include "ColoredCubicSurfaceExtractionTask.h"

#include "Color.h"
#include "OctreeNode.h"

#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/LargeVolume.h"

#include <limits>

using namespace PolyVox;

namespace Cubiquity
{
	ColoredCubicSurfaceExtractionTask::ColoredCubicSurfaceExtractionTask(OctreeNode< Color >* octreeNode, ::PolyVox::POLYVOX_VOLUME<Color>* polyVoxVolume)
		:Task()
		,mOctreeNode(octreeNode)
		,mPolyVoxVolume(polyVoxVolume)
		,mPolyVoxMesh(0)
		,mProcessingStartedTimestamp((std::numeric_limits<Timestamp>::max)())
		,mOwnMesh(false)
	{
	}

	ColoredCubicSurfaceExtractionTask::~ColoredCubicSurfaceExtractionTask()
	{
		if(mOwnMesh)
		{
			delete mPolyVoxMesh;
			mPolyVoxMesh = 0;
			mOwnMesh = false;
		}
	}

	void ColoredCubicSurfaceExtractionTask::process(void)
	{
		mProcessingStartedTimestamp = Clock::getTimestamp();

		Region lod0Region = mOctreeNode->mRegion;

		//Extract the surface
		mPolyVoxMesh = new ::PolyVox::SurfaceMesh<::PolyVox::PositionMaterial<Color> >;
		mOwnMesh = true;

		uint32_t downScaleFactor = 0x0001 << mOctreeNode->mHeight;

		ColoredCubesIsQuadNeeded isQuadNeeded;

		if(downScaleFactor == 1) 
		{
			::PolyVox::CubicSurfaceExtractor< ::PolyVox::POLYVOX_VOLUME<Color>, ColoredCubesIsQuadNeeded > surfaceExtractor(mPolyVoxVolume, mOctreeNode->mRegion, mPolyVoxMesh, ::PolyVox::WrapModes::Border, Color(), true, isQuadNeeded);
			surfaceExtractor.execute();
		}
		else if(downScaleFactor == 2)
		{
		
			Region srcRegion = mOctreeNode->mRegion;

			srcRegion.grow(2);

			Vector3I lowerCorner = srcRegion.getLowerCorner();
			Vector3I upperCorner = srcRegion.getUpperCorner();

			upperCorner = upperCorner - lowerCorner;
			upperCorner = upperCorner / static_cast<int32_t>(downScaleFactor);
			upperCorner = upperCorner + lowerCorner;

			Region dstRegion(lowerCorner, upperCorner);

			::PolyVox::RawVolume<Color> resampledVolume(dstRegion);
			rescaleCubicVolume(mPolyVoxVolume, srcRegion, &resampledVolume, dstRegion);

			dstRegion.shrink(1);
		
			//dstRegion.shiftLowerCorner(-1, -1, -1);

			::PolyVox::CubicSurfaceExtractor< ::PolyVox::RawVolume<Color>, ColoredCubesIsQuadNeeded > surfaceExtractor(&resampledVolume, dstRegion, mPolyVoxMesh, ::PolyVox::WrapModes::Border, Color(), true, isQuadNeeded);
			surfaceExtractor.execute();

			mPolyVoxMesh->scaleVertices(static_cast<float>(downScaleFactor));
			mPolyVoxMesh->translateVertices(Vector3DFloat(0.5f, 0.5f, 0.5f));
		}
		else if(downScaleFactor == 4)
		{
			Region srcRegion = mOctreeNode->mRegion;

			srcRegion.grow(4);

			Vector3I lowerCorner = srcRegion.getLowerCorner();
			Vector3I upperCorner = srcRegion.getUpperCorner();

			upperCorner = upperCorner - lowerCorner;
			upperCorner = upperCorner / static_cast<int32_t>(2);
			upperCorner = upperCorner + lowerCorner;

			Region dstRegion(lowerCorner, upperCorner);

			::PolyVox::RawVolume<Color> resampledVolume(dstRegion);
			rescaleCubicVolume(mPolyVoxVolume, srcRegion, &resampledVolume, dstRegion);



			lowerCorner = dstRegion.getLowerCorner();
			upperCorner = dstRegion.getUpperCorner();

			upperCorner = upperCorner - lowerCorner;
			upperCorner = upperCorner / static_cast<int32_t>(2);
			upperCorner = upperCorner + lowerCorner;

			Region dstRegion2(lowerCorner, upperCorner);

			::PolyVox::RawVolume<Color> resampledVolume2(dstRegion2);
			rescaleCubicVolume(&resampledVolume, dstRegion, &resampledVolume2, dstRegion2);

			dstRegion2.shrink(1);

			//dstRegion.shiftLowerCorner(-1, -1, -1);

			::PolyVox::CubicSurfaceExtractor< ::PolyVox::RawVolume<Color>, ColoredCubesIsQuadNeeded > surfaceExtractor(&resampledVolume2, dstRegion2, mPolyVoxMesh, ::PolyVox::WrapModes::Border, Color(), true, isQuadNeeded);
			surfaceExtractor.execute();

			mPolyVoxMesh->scaleVertices(static_cast<float>(downScaleFactor));
			mPolyVoxMesh->translateVertices(Vector3DFloat(1.5f, 1.5f, 1.5f));
		}

		mOctreeNode->mOctree->mFinishedSurfaceExtractionTasks.push(this);
	}
}
