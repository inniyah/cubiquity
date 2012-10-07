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

	// Builds the graphics mesh for cubic terrain (Material16 material type and PositionMaterial vertex type)
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<PolyVox::Material16> >& polyVoxMesh);
	// Builds the graphics mesh for smooth terrain (MultiMaterial material type and PositionMaterialNormal vertex type)
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< GameplayMarchingCubesController< MultiMaterial >::MaterialType > >& polyVoxMesh);

	// Although we never use Material16 with PositionMaterialNormal or MultiMaterial with PositionMaterial the
	// compiler still needs these to exist because it compiles all combinations. So we just provide dummy functions.
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<MultiMaterial> >& polyVoxMesh);
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< PolyVox::Material16 > >& polyVoxMesh);

	void setMaterial(const char* material); //Should be const material - fix gameplay

	PolyVox::Region mRegion;
	gameplay::Node* mNode;
	bool mIsMeshUpToDate;
};

#endif //VOLUME_REGION_H_
