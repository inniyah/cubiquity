#include "ColoredCubesVolume.h"

#include "Clock.h"
#include "Color.h"
#include "ColoredCubicSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

using namespace PolyVox;

namespace Cubiquity
{
	ColoredCubesVolume* createColoredCubesVolume(const Region& region, const std::string& filename, unsigned int baseNodeSize)
	{
		return new ColoredCubesVolumeImpl(region, filename, baseNodeSize);
	}

	/*ColoredCubesVolumeImpl::ColoredCubesVolumeImpl(const Region& region, const std::string& filename, unsigned int baseNodeSize)
		:Volume<Color>(region, filename, OctreeConstructionModes::BoundVoxels, baseNodeSize)
	{
	}

	void ColoredCubesVolumeImpl::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<Color>::update(viewPosition, lodThreshold);
	}*/
}
