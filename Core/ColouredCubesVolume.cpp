#include "ColouredCubesVolume.h"

#include "Clock.h"
#include "Colour.h"
#include "ColouredCubicSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

using namespace PolyVox;

ColouredCubesVolume::ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	:Volume<Colour>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundVoxels, baseNodeSize)
{
}

void ColouredCubesVolume::update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold)
{
	Volume<Colour>::update(viewPosition, lodThreshold);
}
