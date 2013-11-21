#include "ColoredCubesVolume.h"

#include "Clock.h"
#include "Color.h"
#include "ColoredCubicSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

using namespace PolyVox;

namespace Cubiquity
{
	ColoredCubesVolume* createColoredCubesVolume(const Region& region, const std::string& pathToVoxelDatabase, unsigned int baseNodeSize)
	{
		return new ColoredCubesVolumeImpl(region, pathToVoxelDatabase, baseNodeSize);
	}
}
