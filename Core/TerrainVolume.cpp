#include "TerrainVolume.h"

#include "Clock.h"
#include "MultiMaterial.h"
#include "SmoothSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

#include <algorithm>

using namespace std;
using namespace PolyVox;

namespace Cubiquity
{
	TerrainVolume* createTerrainVolume(const Region& region, const std::string& filename, unsigned int baseNodeSize, bool createFloor, uint32_t floorDepth)
	{
		TerrainVolumeImpl* terrainVolume = new TerrainVolumeImpl(region, filename, baseNodeSize);

		return terrainVolume;
	}

	/*TerrainVolume::TerrainVolume(const Region& region, const std::string& filename, unsigned int baseNodeSize)
		:Volume<MultiMaterial>(region, filename, OctreeConstructionModes::BoundCells, baseNodeSize)
	{
	}

	void TerrainVolume::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<typename MultiMaterialMarchingCubesController::MaterialType>::update(viewPosition, lodThreshold);
	}*/
}