#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "Color.h"
#include "Cubiquity.h"
#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class ColoredCubesVolume : public Volume<Color>
	{
	public:
		typedef Color VoxelType;

		ColoredCubesVolume(const Region& region, const std::string& pathToNewVoxelDatabase, unsigned int baseNodeSize)
			:Volume<Color>(region, pathToNewVoxelDatabase, baseNodeSize)
		{
			setProperty("VoxelType", "Color");

			mOctree = new Octree<VoxelType>(this, OctreeConstructionModes::BoundVoxels, baseNodeSize);
		}

		ColoredCubesVolume(const std::string& pathToExistingVoxelDatabase, unsigned int baseNodeSize)
			:Volume<Color>(pathToExistingVoxelDatabase, baseNodeSize)
		{
			std::string voxelType = getPropertyAsString("VoxelType", "");
			POLYVOX_THROW_IF(voxelType != "Color", std::runtime_error, "VoxelDatabase does not have the expected VoxelType of 'Color'");

			mOctree = new Octree<VoxelType>(this, OctreeConstructionModes::BoundVoxels, baseNodeSize);
		}

		virtual ~ColoredCubesVolume()
		{
			delete mOctree;
		}
	};
}

#endif //COLOUREDCUBESVOLUME_H_