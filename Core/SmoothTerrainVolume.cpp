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
				for(int32_t y = region.getLowerY(); y < region.getUpperY(); y++)
				{
					// Density decreases with increasing y, to create a floor rather than ceiling
					int32_t density = -y;
					// Add the offset to move the floor to the desired level
					density += floorDepth;
					// 'Compress' the density field so that it changes more quickly
					// from fully empty to fully solid (over one a few voxels)
					density *= 64;
					// Account for the threshold not being at zero
					density += MultiMaterial::getMaxMaterialValue() / 2;

					//Clamp resulting density
					density = (std::min)(density, static_cast<int32_t>(MultiMaterial::getMaxMaterialValue()));
					density = (std::max)(density, 0);

					for(int32_t z = region.getLowerZ(); z <= region.getUpperZ(); z++)
					{
						MultiMaterial material;
						material.setMaterial(0, density);
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