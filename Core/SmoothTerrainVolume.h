#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class SmoothTerrainVolume : public Volume<MultiMaterial>
	{

	public:
		SmoothTerrainVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize);

		virtual void update(const Vector3F& viewPosition, float lodThreshold);

	};
}

#endif //SMOOTHTERRAINVOLUME_H_
