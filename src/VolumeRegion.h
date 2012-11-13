#ifndef VOLUME_REGION_H_
#define VOLUME_REGION_H_

#include "Node.h"
#include "Ref.h"

#include "Colour.h"

#include "GameplayMarchingCubesController.h"
#include "MultiMaterial.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceMesh.h"

template <typename VoxelType>
class Volume;

class VolumeRegion
{
public:	
	static const uint32_t NoOfLodLevels = 3;

	VolumeRegion(PolyVox::Region region, gameplay::Node* parentNode);
	~VolumeRegion();

	// Builds the graphics mesh for cubic terrain (Colour material type and PositionMaterial vertex type)
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<Colour> >& polyVoxMesh, uint32_t lod);
	// Builds the graphics mesh for smooth terrain (MultiMaterial material type and PositionMaterialNormal vertex type)
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< GameplayMarchingCubesController< MultiMaterial4 >::MaterialType > >& polyVoxMesh, uint32_t lod);

	// Although we never use Colour with PositionMaterialNormal or MultiMaterial with PositionMaterial the
	// compiler still needs these to exist because it compiles all combinations. So we just provide dummy functions.
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<MultiMaterial4> >& polyVoxMesh, uint32_t lod);
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< Colour > >& polyVoxMesh, uint32_t lod);

	void setMaterial(const char* material); //Should be const material - fix gameplay

	PolyVox::Region mRegion;
	gameplay::Node* mNode[NoOfLodLevels];
	bool mIsMeshUpToDate;
};

#endif //VOLUME_REGION_H_
