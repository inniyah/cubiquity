#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "ColouredCubicSurfaceExtractionTask.h"
#include "Volume.h"

#include "Colour.h"
#include "PolyVoxCore/Material.h"

#include <list>

class ColouredCubesVolume : public Volume<Colour>
{
public:
	ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);

	void updateMeshImpl(OctreeNode* volReg);

	//void generateCubicMesh(const PolyVox::Region& region, uint32_t downSampleFactor, PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >* resultMesh);

	std::list<ColouredCubicSurfaceExtractionTask> mCubicSurfaceExtractionTasks;
};

#endif //COLOUREDCUBESVOLUME_H_
