#ifndef VOLUME_REGION_H_
#define VOLUME_REGION_H_

#include "Node.h"
#include "Ref.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

class Volume;

class VolumeRegion
{
public:	
	VolumeRegion(const Volume* volume, PolyVox::Region region);
	~VolumeRegion();

	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& polyVoxMesh);
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal>& polyVoxMesh);

	const Volume* mVolume;
	PolyVox::Region mRegion;
	gameplay::Node* mNode;
};

#endif //VOLUME_REGION_H_
