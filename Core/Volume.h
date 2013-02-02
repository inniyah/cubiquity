#ifndef VOLUME_H_
#define VOLUME_H_

#include "OctreeNode.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"
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

	void buildOctreeNodeTree(OctreeNode* parent, const PolyVox::Region& regionToCover, OctreeConstructionMode octreeConstructionMode);


	VoxelType getVoxelAt(int x, int y, int z);
	void setVoxelAt(int x, int y, int z, VoxelType value, bool markAsModified = true);
	void markRegionAsModified(const PolyVox::Region& region);

	void update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	void updateMesh(OctreeNode* volReg);
	virtual void updateMeshImpl(OctreeNode* volReg) = 0;

	uint32_t getTime(void)
	{
		POLYVOX_ASSERT(mTime < std::numeric_limits<uint32_t>::max(), "Time stamp is wrapping around.");
		return ++mTime;
	}

protected:
	Volume& operator=(const Volume&);

public:
	PolyVox::RawVolume<VoxelType>* mVolData;
	OctreeNode* mRootOctreeNode;

	const unsigned int mBaseNodeSize;

	uint32_t mTime;
};

#include "Volume.inl"

#endif //VOLUME_H_
