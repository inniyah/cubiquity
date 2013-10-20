#ifndef TERRAINVOLUMEGENERATOR_H_
#define TERRAINVOLUMEGENERATOR_H_

#include "CubiquityForwardDeclarations.h"
#include "Vector.h"

namespace Cubiquity
{
	void generateFloor(TerrainVolume* terrainVolume, int32_t lowerLayerHeight, uint32_t lowerLayerMaterial, int32_t upperLayerHeight, uint32_t upperLayerMaterial);
}

#endif //TERRAINVOLUMEGENERATOR_H_
