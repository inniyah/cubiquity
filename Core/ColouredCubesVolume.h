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
		ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);

		virtual void update(const ::PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
		void updateMeshImpl(OctreeNode< VoxelType >* octreeNode);
	};
}

#endif //COLOUREDCUBESVOLUME_H_
