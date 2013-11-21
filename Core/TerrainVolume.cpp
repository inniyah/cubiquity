#include "TerrainVolume.h"

#include "Clock.h"
#include "MaterialSet.h"
#include "SmoothSurfaceExtractionTask.h"
#include "MainThreadTaskProcessor.h"

#include <algorithm>

using namespace std;
using namespace PolyVox;

namespace Cubiquity
{
	TerrainVolume* createTerrainVolume(const Region& region, const std::string& pathToVoxelDatabase, unsigned int baseNodeSize, bool createFloor, uint32_t floorDepth)
	{
		TerrainVolumeImpl* terrainVolume = new TerrainVolumeImpl(region, pathToVoxelDatabase, baseNodeSize);

		return terrainVolume;
	}
}