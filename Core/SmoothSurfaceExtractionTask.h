#ifndef CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_
#define CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_

#include "CubiquityForwardDeclarations.h"
#include "OctreeNode.h"
#include "Task.h"

namespace Cubiquity
{
	class SmoothSurfaceExtractionTask : public Task
	{
	public:
		SmoothSurfaceExtractionTask(OctreeNode< MaterialSet >* octreeNode, ::PolyVox::POLYVOX_VOLUME< typename MaterialSetMarchingCubesController::MaterialType >* polyVoxVolume);
		~SmoothSurfaceExtractionTask();

		void process(void);

		void generateSmoothMesh(const Region& region, uint32_t lodLevel, ::PolyVox::SurfaceMesh<::PolyVox::PositionMaterialNormal< typename MaterialSetMarchingCubesController::MaterialType > >* resultMesh);

	public:
		OctreeNode< MaterialSet >* mOctreeNode;
		::PolyVox::POLYVOX_VOLUME<typename MaterialSetMarchingCubesController::MaterialType>* mPolyVoxVolume;
		::PolyVox::SurfaceMesh<::PolyVox::PositionMaterialNormal< typename MaterialSetMarchingCubesController::MaterialType> >* mPolyVoxMesh;
		Timestamp mProcessingStartedTimestamp;

		// Whether the task owns the mesh, or whether it has been passed to
		// the OctreeNode. Should probably switch this to use a smart pointer.
		bool mOwnMesh;
	};

	void recalculateMaterials(::PolyVox::SurfaceMesh<::PolyVox::PositionMaterialNormal< typename MaterialSetMarchingCubesController::MaterialType > >* mesh, const Vector3F& meshOffset, ::PolyVox::POLYVOX_VOLUME<MaterialSet>* volume);
	MaterialSet getInterpolatedValue(::PolyVox::POLYVOX_VOLUME<MaterialSet>* volume, const Vector3F& position);

	template< typename SrcPolyVoxVolumeType, typename DstPolyVoxVolumeType>
	void resampleVolume(uint32_t factor, SrcPolyVoxVolumeType* srcVolume, const Region& srcRegion, DstPolyVoxVolumeType* dstVolume, const Region& dstRegion)
	{
		POLYVOX_ASSERT(srcRegion.getWidthInCells() == dstRegion.getWidthInCells() * factor, "Destination volume must be half the size of source volume");
		POLYVOX_ASSERT(srcRegion.getHeightInCells() == dstRegion.getHeightInCells() * factor, "Destination volume must be half the size of source volume");
		POLYVOX_ASSERT(srcRegion.getDepthInCells() == dstRegion.getDepthInCells() * factor, "Destination volume must be half the size of source volume");

		for(int32_t dz = dstRegion.getLowerCorner().getZ(); dz <= dstRegion.getUpperCorner().getZ(); dz++)
		{
			for(int32_t dy = dstRegion.getLowerCorner().getY(); dy <= dstRegion.getUpperCorner().getY(); dy++)
			{
				for(int32_t dx = dstRegion.getLowerCorner().getX(); dx <= dstRegion.getUpperCorner().getX(); dx++)
				{
					int32_t sx = (dx - dstRegion.getLowerCorner().getX()) * factor + srcRegion.getLowerCorner().getX();
					int32_t sy = (dy - dstRegion.getLowerCorner().getY()) * factor + srcRegion.getLowerCorner().getY();
					int32_t sz = (dz - dstRegion.getLowerCorner().getZ()) * factor + srcRegion.getLowerCorner().getZ();

					const MaterialSet& srcVoxel = srcVolume->getVoxel<WrapModes::Border>(sx,sy,sz);
					dstVolume->setVoxelAt(dx,dy,dz,srcVoxel);
				}
			}
		}
	}
}

#endif //CUBIQUITY_SMOOTHSURFACEEXTRACTIONTASK_H_
