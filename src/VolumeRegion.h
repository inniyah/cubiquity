#ifndef VOLUME_REGION_H_
#define VOLUME_REGION_H_

#include "Node.h"
#include "Ref.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

class VolumeRegion
{
public:	
	VolumeRegion(PolyVox::Region region);
	~VolumeRegion();

	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial>& polyVoxMesh);

	PolyVox::Region mRegion;
	gameplay::Node* mNode;
};

#endif //VOLUME_REGION_H_
