#include "ColouredCubesVolume.h"

#include "Clock.h"
#include "Colour.h"
#include "ColouredCubicSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

using namespace PolyVox;

namespace Cubiquity
{
	ColouredCubesVolume::ColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
		:Volume<Colour>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundVoxels, baseNodeSize)
	{
	}

	void ColouredCubesVolume::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<Colour>::update(viewPosition, lodThreshold);
	}
}
