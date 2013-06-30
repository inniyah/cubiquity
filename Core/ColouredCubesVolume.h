#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "Colour.h"
#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class ColouredCubesVolume : public Volume<Colour>
	{
	public:
		ColouredCubesVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize);

		virtual void update(const Vector3F& viewPosition, float lodThreshold);
		void updateMeshImpl(OctreeNode< VoxelType >* octreeNode);
	};
}

#endif //COLOUREDCUBESVOLUME_H_
