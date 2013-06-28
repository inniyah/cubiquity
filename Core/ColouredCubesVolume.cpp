#include "ColouredCubesVolume.h"

#include "Clock.h"
#include "Colour.h"
#include "ColouredCubicSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

using namespace PolyVox;

namespace Cubiquity
{
	ColouredCubesVolume::ColouredCubesVolume(const Region& region, unsigned int baseNodeSize, const std::string& pageFolder)
		:Volume<Colour>(region, OctreeConstructionModes::BoundVoxels, baseNodeSize, pageFolder)
	{
	}

	void ColouredCubesVolume::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<Colour>::update(viewPosition, lodThreshold);
	}
}
