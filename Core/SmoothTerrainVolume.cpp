#include "SmoothTerrainVolume.h"

#include "Clock.h"
#include "MultiMaterial.h"
#include "SmoothSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

#include <algorithm>

using namespace std;
using namespace PolyVox;

namespace Cubiquity
{
	SmoothTerrainVolume::SmoothTerrainVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize)
		:Volume<MultiMaterial>(region, pageFolder, OctreeConstructionModes::BoundCells, baseNodeSize)
	{
	}

	void SmoothTerrainVolume::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<typename MultiMaterialMarchingCubesController::MaterialType>::update(viewPosition, lodThreshold);
	}
}