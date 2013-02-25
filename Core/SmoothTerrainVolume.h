#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

class SmoothTerrainVolume : public Volume<MultiMaterial>
{

public:
	SmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);

	virtual void update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	void updateMeshImpl(OctreeNode< typename VoxelTraits<VoxelType>::VertexType >* octreeNode);
};

#endif //SMOOTHTERRAINVOLUME_H_
