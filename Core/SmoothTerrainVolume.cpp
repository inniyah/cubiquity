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
	SmoothTerrainVolume::SmoothTerrainVolume(const Region& region, unsigned int baseNodeSize, const std::string& folderName)
		:Volume<MultiMaterial>(region, OctreeConstructionModes::BoundCells, baseNodeSize, folderName)
	{
	}

	void SmoothTerrainVolume::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<typename MultiMaterialMarchingCubesController::MaterialType>::update(viewPosition, lodThreshold);
	}
}