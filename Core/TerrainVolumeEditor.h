#ifndef TERRAINVOLUMEEDITOR_H_
#define TERRAINVOLUMEEDITOR_H_

#include "CubiquityForwardDeclarations.h"
#include "Region.h"
#include "Vector.h"

namespace Cubiquity
{
	void sculptTerrainVolume(TerrainVolumeImpl* terrainVolume, const Vector3F& centre, const Brush& brush);
	void blurTerrainVolume(TerrainVolumeImpl* terrainVolume, const Vector3F& centre, const Brush& brush);
	void blurTerrainVolume(TerrainVolumeImpl* terrainVolume, const Region& region);
	void paintTerrainVolume(TerrainVolumeImpl* terrainVolume, const Vector3F& centre, const Brush& brush, uint32_t materialIndex);
}

#endif //TERRAINVOLUMEEDITOR_H_
