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
	SmoothTerrainVolume::SmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
		:Volume<MultiMaterial>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, OctreeConstructionModes::BoundCells, baseNodeSize)
	{
	}

	void SmoothTerrainVolume::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<typename MultiMaterialMarchingCubesController::MaterialType>::update(viewPosition, lodThreshold);
	}
}