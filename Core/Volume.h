#ifndef VOLUME_H_
#define VOLUME_H_

#include "OctreeNode.h"
#include "VoxelTraits.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

namespace OctreeConstructionModes
{
	enum OctreeConstructionMode
	{
		BoundVoxels = 0,
		BoundCells = 1
	};
}
typedef OctreeConstructionModes::OctreeConstructionMode OctreeConstructionMode;

template <typename _VoxelType>
class Volume
{
public:
	typedef _VoxelType VoxelType;

	Volume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize);
	~Volume();

	void buildOctreeNodeTree(OctreeNode< typename VoxelTraits<VoxelType>::VertexType >* parent, const PolyVox::Region& regionToCover, OctreeConstructionMode octreeConstructionMode);


	VoxelType getVoxelAt(int x, int y, int z);
	void setVoxelAt(int x, int y, int z, VoxelType value, bool markAsModified = true);
	void markRegionAsModified(const PolyVox::Region& region);

	virtual void update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	void updateMesh(OctreeNode< typename VoxelTraits<VoxelType>::VertexType >* octreeNode);
	virtual void updateMeshImpl(OctreeNode< typename VoxelTraits<VoxelType>::VertexType >* octreeNode) = 0;

protected:
	Volume& operator=(const Volume&);

public:
	PolyVox::SimpleVolume<VoxelType>* mPolyVoxVolume;
	OctreeNode< typename VoxelTraits<VoxelType>::VertexType >* mRootOctreeNode;

	const unsigned int mBaseNodeSize;
};

#include "Volume.inl"

#endif //VOLUME_H_
