#ifndef VOLUME_REGION_H_
#define VOLUME_REGION_H_

#include "Node.h"
#include "Ref.h"

#include "GameplayMarchingCubesController.h"
#include "MultiMaterial.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

template <typename VoxelType>
class Volume;

class VolumeRegion
{
public:	
	VolumeRegion(PolyVox::Region region);
	~VolumeRegion();

	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<PolyVox::Material16> >& polyVoxMesh);
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< GameplayMarchingCubesController< MultiMaterial >::MaterialType > >& polyVoxMesh);

	void setMaterial(const char* material); //Should be const material - fix gameplay

	PolyVox::Region mRegion;
	gameplay::Node* mNode;
	bool mIsMeshUpToDate;
};

#endif //VOLUME_REGION_H_
