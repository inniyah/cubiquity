#include "ColouredCubesVolume.h"

#include "Clock.h"
#include "Colour.h"
#include "ColouredCubicSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

using namespace PolyVox;

namespace Cubiquity
{
	ColouredCubesVolume* createColoredCubesVolume(const Region& region, const std::string& filename, unsigned int baseNodeSize)
	{
		return new ColouredCubesVolumeImpl(region, filename, baseNodeSize);
	}

	/*ColouredCubesVolumeImpl::ColouredCubesVolumeImpl(const Region& region, const std::string& filename, unsigned int baseNodeSize)
		:Volume<Colour>(region, filename, OctreeConstructionModes::BoundVoxels, baseNodeSize)
	{
	}

	void ColouredCubesVolumeImpl::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<Colour>::update(viewPosition, lodThreshold);
	}*/
}
