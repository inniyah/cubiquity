#ifndef VOLUME_REGION_H_
#define VOLUME_REGION_H_

#include "Node.h"
#include "Ref.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

template <typename VoxelType>
class Volume;

template <typename VoxelType>
class VolumeRegion
{
public:	
	VolumeRegion(const Volume<VoxelType>* volume, PolyVox::Region region);
	~VolumeRegion();

	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& polyVoxMesh);
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyVoxMesh);

	const Volume<VoxelType>* mVolume;
	PolyVox::Region mRegion;
	gameplay::Node* mNode;
};

#include "VolumeRegion.inl"

#endif //VOLUME_REGION_H_
