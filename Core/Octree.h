#ifndef CUBIQUITY_OCTREE_H_
#define CUBIQUITY_OCTREE_H_

#include "CubiquityForwardDeclarations.h"

namespace OctreeConstructionModes
{
	enum OctreeConstructionMode
	{
		BoundVoxels = 0,
		BoundCells = 1
	};
}
typedef OctreeConstructionModes::OctreeConstructionMode OctreeConstructionMode;

template <typename VoxelType>
class Octree
{
public:
	Octree(Volume<VoxelType>* volume, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize);

	void buildOctreeNodeTree(OctreeNode< VoxelType >* parent, const PolyVox::Region& regionToCover, OctreeConstructionMode octreeConstructionMode);

	Volume<VoxelType>* mVolume;
	OctreeNode<VoxelType>* mRootOctreeNode;
	const unsigned int mBaseNodeSize;
};

#include "Octree.inl"

#endif //CUBIQUITY_OCTREE_H_
