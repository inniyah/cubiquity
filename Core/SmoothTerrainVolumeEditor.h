#ifndef SMOOTHTERRAINVOLUMEEDITOR_H_
#define SMOOTHTERRAINVOLUMEEDITOR_H_

#include "CubiquityForwardDeclarations.h"
#include "Vector.h"

namespace Cubiquity
{
	void sculptSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, const Brush& brush);
	void blurSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float brushRadius, float amount);
	void paintSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float brushRadius, uint32_t materialIndex, float amount);
}

#endif //SMOOTHTERRAINVOLUMEEDITOR_H_
