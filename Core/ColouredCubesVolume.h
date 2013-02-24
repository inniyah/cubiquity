#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

class ColouredCubesVolume : public Volume<Colour>
{
public:
	ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);

	virtual void update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	void updateMeshImpl(OctreeNode< typename VoxelTraits<VoxelType>::VertexType >* octreeNode);

public:
	//This task processor must be created and set by out engine-specific derived class.
	TaskProcessor<ColouredCubicSurfaceExtractionTask>* mColouredCubicSurfaceExtractionTaskProcessor;
};

#endif //COLOUREDCUBESVOLUME_H_
