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
	SmoothTerrainVolume* createSmoothTerrainVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize, bool createFloor, uint32_t floorDepth)
	{
		SmoothTerrainVolumeImpl* smoothTerrainVolume = new SmoothTerrainVolumeImpl(region, pageFolder, baseNodeSize);

		if(createFloor)
		{
			for(int32_t x = region.getLowerX(); x <= region.getUpperX(); x++)
			{
				for(int32_t y = region.getLowerY(); y < region.getLowerY() + floorDepth; y++)
				{
					for(int32_t z = region.getLowerZ(); z <= region.getUpperZ(); z++)
					{
						MultiMaterial material;
						material.setMaterial(0, MultiMaterial::getMaxMaterialValue());
						smoothTerrainVolume->setVoxelAt(x, y, z, material, UpdatePriorities::DontUpdate);
					}
				}
			}

			// We don't need to mark the volume as modified after writing the floor because it's a new volume.
		}

		return smoothTerrainVolume;
	}

	/*SmoothTerrainVolume::SmoothTerrainVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize)
		:Volume<MultiMaterial>(region, pageFolder, OctreeConstructionModes::BoundCells, baseNodeSize)
	{
	}

	void SmoothTerrainVolume::update(const Vector3F& viewPosition, float lodThreshold)
	{
		Volume<typename MultiMaterialMarchingCubesController::MaterialType>::update(viewPosition, lodThreshold);
	}*/
}