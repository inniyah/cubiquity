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
			:Volume<MaterialSet>(region, pathToVoxelDatabase, baseNodeSize)
		{
			mOctree = new Octree<VoxelType>(this, OctreeConstructionModes::BoundCells, baseNodeSize);
		}

		virtual ~TerrainVolume()
		{
			delete mOctree;
		}
	};
}

#endif //TERRAINVOLUME_H_
