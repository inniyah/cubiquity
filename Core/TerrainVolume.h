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

		TerrainVolume(const Region& region, const std::string& pathToNewVoxelDatabase, unsigned int baseNodeSize)
			:Volume<MaterialSet>(region, pathToNewVoxelDatabase, baseNodeSize)
		{
			mOctree = new Octree<VoxelType>(this, OctreeConstructionModes::BoundCells, baseNodeSize);
		}

		TerrainVolume(const std::string& pathToExistingVoxelDatabase, unsigned int baseNodeSize)
			:Volume<MaterialSet>(pathToExistingVoxelDatabase, baseNodeSize)
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
