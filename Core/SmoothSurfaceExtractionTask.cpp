#include "SmoothSurfaceExtractionTask.h"

#include "MaterialSet.h"

#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/LargeVolume.h"

#include <limits>

using namespace PolyVox;

namespace Cubiquity
{
	SmoothSurfaceExtractionTask::SmoothSurfaceExtractionTask(OctreeNode< MaterialSet >* octreeNode, ::PolyVox::LargeVolume<typename MaterialSetMarchingCubesController::MaterialType>* polyVoxVolume)
		:Task()
		,mOctreeNode(octreeNode)
		,mPolyVoxVolume(polyVoxVolume)
		,mPolyVoxMesh(0)
		,mProcessingStartedTimestamp((std::numeric_limits<Timestamp>::max)())
		,mOwnMesh(false)
	{
	}

	SmoothSurfaceExtractionTask::~SmoothSurfaceExtractionTask()
	{
		if(mOwnMesh)
		{
			delete mPolyVoxMesh;
			mPolyVoxMesh = 0;
			mOwnMesh = false;
		}
	}

	void SmoothSurfaceExtractionTask::process(void)
	{
		mProcessingStartedTimestamp = Clock::getTimestamp();
		//Extract the surface
		mPolyVoxMesh = new ::PolyVox::SurfaceMesh<::PolyVox::PositionMaterialNormal< typename MaterialSetMarchingCubesController::MaterialType > >;
		mOwnMesh = true;

		generateSmoothMesh(mOctreeNode->mRegion, mOctreeNode->mHeight, mPolyVoxMesh);

		mOctreeNode->mOctree->mFinishedSurfaceExtractionTasks.push(this);
	}

	void SmoothSurfaceExtractionTask::generateSmoothMesh(const Region& region, uint32_t lodLevel, ::PolyVox::SurfaceMesh<::PolyVox::PositionMaterialNormal< typename MaterialSetMarchingCubesController::MaterialType > >* resultMesh)
	{
		MaterialSetMarchingCubesController controller;

		if(lodLevel == 0)
		{
			//SurfaceMesh<PositionMaterialNormal< typename MaterialSetMarchingCubesController::MaterialType > > mesh;
			::PolyVox::MarchingCubesSurfaceExtractor< ::PolyVox::LargeVolume<MaterialSet>, MaterialSetMarchingCubesController > surfaceExtractor(mPolyVoxVolume, region, resultMesh, ::PolyVox::WrapModes::Border, MaterialSet(0), controller);
			surfaceExtractor.execute();
		}
		else
		{
			uint32_t downSampleFactor = 0x0001 << lodLevel;

			int crackHidingFactor = 5; //This should probably be configurable?
			controller.setThreshold(controller.getThreshold() + (downSampleFactor * crackHidingFactor));

			Region highRegion = region;
			highRegion.grow(downSampleFactor, downSampleFactor, downSampleFactor);

			Region lowRegion = highRegion;
			Vector3I lowerCorner = lowRegion.getLowerCorner();
			Vector3I upperCorner = lowRegion.getUpperCorner();

			upperCorner = upperCorner - lowerCorner;
			upperCorner = upperCorner / static_cast<int32_t>(downSampleFactor);
			upperCorner = upperCorner + lowerCorner;
			lowRegion.setUpperCorner(upperCorner);

			::PolyVox::RawVolume<MaterialSet> resampledVolume(lowRegion);

			resampleVolume(downSampleFactor, mPolyVoxVolume, highRegion, &resampledVolume, lowRegion);

			lowRegion.shrink(1, 1, 1);

			::PolyVox::MarchingCubesSurfaceExtractor< ::PolyVox::RawVolume<MaterialSet>, MaterialSetMarchingCubesController > surfaceExtractor(&resampledVolume, lowRegion, resultMesh, ::PolyVox::WrapModes::Border, MaterialSet(0), controller);
			surfaceExtractor.execute();

			resultMesh->scaleVertices(static_cast<float>(downSampleFactor));

			recalculateMaterials(resultMesh, static_cast<Vector3F>(mOctreeNode->mRegion.getLowerCorner()), mPolyVoxVolume);
		}
	}

	void recalculateMaterials(::PolyVox::SurfaceMesh<::PolyVox::PositionMaterialNormal< typename MaterialSetMarchingCubesController::MaterialType > >* mesh, const Vector3F& meshOffset,  ::PolyVox::LargeVolume<MaterialSet>* volume)
	{
		std::vector< PositionMaterialNormal< typename MaterialSetMarchingCubesController::MaterialType > >& vertices = mesh->getRawVertexData();
		for(uint32_t ct = 0; ct < vertices.size(); ct++)
		{
			const Vector3DFloat& vertexPos = vertices[ct].getPosition() + meshOffset;
			MaterialSet value = getInterpolatedValue(volume, vertexPos);

			// It seems that sometimes the vertices can fall in an empty cell. The reason for this
			// isn't clear but it might be inaccuraceies in the lower LOD mesh. It also might only 
			// happen right on the edge of the volume so wrap modes might help. Hopefully we can
			// remove this hack in the future.
			Vector<8, float> matAsVec = value;
			if(matAsVec.length() < 0.001f)
			{
				value = MaterialSet(0);
				value.setMaterial(0, 255);
			}

			vertices[ct].setMaterial(value);
		}
	}


	MaterialSet getInterpolatedValue(::PolyVox::LargeVolume<MaterialSet>* volume, const Vector3F& position)
	{
		::PolyVox::LargeVolume<MaterialSet>::Sampler sampler(volume);

		int32_t iLowerX = ::PolyVox::roundTowardsNegInf(position.getX());
		int32_t iLowerY = ::PolyVox::roundTowardsNegInf(position.getY());
		int32_t iLowerZ = ::PolyVox::roundTowardsNegInf(position.getZ());

		float fOffsetX = position.getX() - iLowerX;
		float fOffsetY = position.getY() - iLowerY;
		float fOffsetZ = position.getZ() - iLowerZ;

		/*int32_t iCeilX = iFloorX + 1;
		int32_t iCeilY = iFloorY + 1;
		int32_t iCeilZ = iFloorZ + 1;*/

		sampler.setPosition(iLowerX, iLowerY, iLowerZ);

		MaterialSet v000 = sampler.peekVoxel0px0py0pz();
		MaterialSet v100 = sampler.peekVoxel1px0py0pz();
		MaterialSet v010 = sampler.peekVoxel0px1py0pz();
		MaterialSet v110 = sampler.peekVoxel1px1py0pz();
		MaterialSet v001 = sampler.peekVoxel0px0py1pz();
		MaterialSet v101 = sampler.peekVoxel1px0py1pz();
		MaterialSet v011 = sampler.peekVoxel0px1py1pz();
		MaterialSet v111 = sampler.peekVoxel1px1py1pz();

		MaterialSet result = trilerp(v000, v100, v010, v110, v001, v101, v011, v111, fOffsetX, fOffsetY, fOffsetZ);

		return result;
	}
}
