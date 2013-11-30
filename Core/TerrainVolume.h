#ifndef TERRAINVOLUME_H_
#define TERRAINVOLUME_H_

#include "Cubiquity.h"
#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class TerrainVolume : public Volume<MaterialSet>
	{
	public:
		typedef MaterialSet VoxelType;

		TerrainVolume(const Region& region, const std::string& pathToVoxelDatabase, unsigned int baseNodeSize)
			:Volume<MaterialSet>(region, pathToVoxelDatabase, OctreeConstructionModes::BoundCells, baseNodeSize)
		{
		}

		virtual ~TerrainVolume()
		{
		}
	};
}

#endif //TERRAINVOLUME_H_
