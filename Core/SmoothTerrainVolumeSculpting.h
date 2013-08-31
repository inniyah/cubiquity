#ifndef CUBIQUITY_SMOOTHTERRAINVOLUMESCULPTING_H
#define CUBIQUITY_SMOOTHTERRAINVOLUMESCULPTING_H

#include "CubiquityForwardDeclarations.h"
#include "Vector.h"

namespace Cubiquity
{
	void sculptSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float brushRadius, float speed);
}

#endif //CUBIQUITY_SMOOTHTERRAINVOLUMESCULPTING_H
