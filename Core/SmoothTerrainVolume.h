#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "MultiMaterial.h"
#include "Volume.h"

#include "PolyVoxCore/MaterialDensityPair.h"

template< typename SrcVolumeType, typename DstVolumeType>
void resampleVolume(uint32_t factor, SrcVolumeType* srcVolume, const PolyVox::Region& srcRegion, DstVolumeType* dstVolume, const PolyVox::Region& dstRegion)
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

				const MultiMaterial4& srcVoxel = srcVolume->getVoxelWithWrapping(sx,sy,sz);
				dstVolume->setVoxelAt(dx,dy,dz,srcVoxel);
			}
		}
	}
}

class SmoothTerrainVolume : public Volume<MultiMaterial4>
{

public:
	SmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);

	void updateMeshImpl(OctreeNode* volReg);

	void generateSmoothMesh(const PolyVox::Region& region, uint32_t lodLevel, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >* resultMesh);

	void recalculateMaterials(PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType > >* mesh, const PolyVox::Vector3DFloat& meshOffset, PolyVox::SimpleVolume<MultiMaterial4>* volume);
	MultiMaterial4 getInterpolatedValue(PolyVox::SimpleVolume<MultiMaterial4>* volume, const PolyVox::Vector3DFloat& position);
};

#endif //SMOOTHTERRAINVOLUME_H_
