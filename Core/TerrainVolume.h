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
			setProperty("VoxelType", "MaterialSet");

			mOctree = new Octree<VoxelType>(this, OctreeConstructionModes::BoundCells, baseNodeSize);
		}

		TerrainVolume(const std::string& pathToExistingVoxelDatabase, unsigned int baseNodeSize)
			:Volume<MaterialSet>(pathToExistingVoxelDatabase, baseNodeSize)
		{
			std::string voxelType = getPropertyAsString("VoxelType", "");
			POLYVOX_THROW_IF(voxelType != "MaterialSet", std::runtime_error, "VoxelDatabase does not have the expected VoxelType of 'MaterialSet'");

			mOctree = new Octree<VoxelType>(this, OctreeConstructionModes::BoundCells, baseNodeSize);
		}

		virtual ~TerrainVolume()
		{
			delete mOctree;
		}
	};
}

#endif //TERRAINVOLUME_H_
