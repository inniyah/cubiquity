#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class SmoothTerrainVolume : public Volume<MultiMaterial>
	{

	public:
		SmoothTerrainVolume(const Region& region, unsigned int blockSize, unsigned int baseNodeSize);

		virtual void update(const Vector3F& viewPosition, float lodThreshold);
		void updateMeshImpl(OctreeNode< VoxelType >* octreeNode);
	};
}

#endif //SMOOTHTERRAINVOLUME_H_
