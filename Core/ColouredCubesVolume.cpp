#include "ColouredCubesVolume.h"

#include "Clock.h"
#include "Colour.h"
#include "ColouredCubicSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

using namespace PolyVox;

namespace Cubiquity
{
	ColouredCubesVolume::ColouredCubesVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize)
		:Volume<Colour>(region, pageFolder, OctreeConstructionModes::BoundVoxels, baseNodeSize)
	{
	}

	void ColouredCubesVolume::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<Colour>::update(viewPosition, lodThreshold);
	}
}
