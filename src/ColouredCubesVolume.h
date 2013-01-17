#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "Volume.h"

#include "Colour.h"
#include "PolyVoxCore/Material.h"

void rescaleCubicVolume(PolyVox::RawVolume<Colour>* pVolSrc, const PolyVox::Region& regSrc, PolyVox::RawVolume<Colour>* pVolDst, const PolyVox::Region& regDst);

class ColouredCubesVolume : public Volume<Colour>
{
public:
	ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int baseNodeSize);

	void updateMeshImpl(OctreeNode* volReg);

	void generateCubicMesh(const PolyVox::Region& region, uint32_t downSampleFactor, PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >* resultMesh);
};

#endif //COLOUREDCUBESVOLUME_H_
