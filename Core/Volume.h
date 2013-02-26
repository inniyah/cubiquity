#ifndef VOLUME_H_
#define VOLUME_H_

#include "Octree.h"
#include "VoxelTraits.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

template <typename _VoxelType>
class Volume
{
public:
	typedef _VoxelType VoxelType;

	Volume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize);
	~Volume();


	VoxelType getVoxelAt(int x, int y, int z);
	void setVoxelAt(int x, int y, int z, VoxelType value, bool markAsModified = true);
	void markRegionAsModified(const PolyVox::Region& region);

	virtual void update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
protected:
	Volume& operator=(const Volume&);

public:
	PolyVox::SimpleVolume<VoxelType>* mPolyVoxVolume;
	//OctreeNode< VoxelType >* mRootOctreeNode;
	Octree<VoxelType>* mOctree;
};

#include "Volume.inl"

#endif //VOLUME_H_
