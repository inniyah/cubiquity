#ifndef VOLUME_H_
#define VOLUME_H_

#include "OctreeNode.h"

#include "Node.h"
#include "Ref.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

namespace VolumeTypes
{
	enum VolumeType
	{
		ColouredCubes = 0,
		SmoothTerrain = 1
	};
}
typedef VolumeTypes::VolumeType VolumeType;

template <typename _VoxelType>
class Volume
{
public:
	Volume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth, unsigned int baseNodeSize = 16);
	~Volume();

	typedef _VoxelType VoxelType;

	VolumeType getType(void) const;

	void buildOctreeNodeTree(OctreeNode* parent);

	VoxelType getVoxelAt(int x, int y, int z);
	void setVoxelAt(int x, int y, int z, VoxelType value);

	bool raycast(gameplay::Ray ray, float distance, gameplay::Vector3& result);

	void update(const PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	void updateMesh(OctreeNode* volReg);

	void recalculateMaterials(PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >* mesh, const PolyVox::Vector3DFloat& meshOffset, PolyVox::RawVolume<VoxelType>* volume);
	VoxelType getInterpolatedValue(PolyVox::RawVolume<VoxelType>* volume, const PolyVox::Vector3DFloat& position);

	void generateSmoothMesh(const PolyVox::Region& region, uint32_t downSampleFactor, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >* resultMesh);
	void generateCubicMesh(const PolyVox::Region& region, uint32_t downSampleFactor, PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >* resultMesh);

	uint32_t getTime(void)
	{
		POLYVOX_ASSERT(mTime < std::numeric_limits<uint32_t>::max(), "Time stamp is wrapping around.");
		return ++mTime;
	}

protected:
	Volume& operator=(const Volume&);

	VolumeType mType;

public:
	PolyVox::RawVolume<VoxelType>* mVolData;
	OctreeNode* mRootOctreeNode;
	std::string mMaterialPath;

	const unsigned int mRegionWidth;
	const unsigned int mRegionHeight;
	const unsigned int mRegionDepth;

	const unsigned int mBaseNodeSize;

	uint32_t mTime;
};

#include "Volume.inl"

#endif //VOLUME_H_
