#ifndef TERRAINVOLUMEEDITOR_H_
#define TERRAINVOLUMEEDITOR_H_

#include "CubiquityForwardDeclarations.h"
#include "Region.h"
#include "Vector.h"

namespace Cubiquity
{
	void sculptTerrainVolume(TerrainVolume* terrainVolume, const Vector3F& centre, const Brush& brush);
	void blurTerrainVolume(TerrainVolume* terrainVolume, const Vector3F& centre, const Brush& brush);
	void blurTerrainVolume(TerrainVolume* terrainVolume, const Region& region);
	void paintTerrainVolume(TerrainVolume* terrainVolume, const Vector3F& centre, const Brush& brush, uint32_t materialIndex);
}

#endif //TERRAINVOLUMEEDITOR_H_
