#ifndef VOLUME_H_
#define VOLUME_H_

#include "VolumeRegion.h"

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

template <typename VoxelType>
class Volume : public gameplay::Ref
{
public:	
	gameplay::Node* getRootNode();
	VolumeType getType(void) const;

	VoxelType getVoxelAt(int x, int y, int z);
	void setVoxelAt(int x, int y, int z, VoxelType value);

	bool raycast(gameplay::Ray ray, float distance, gameplay::Vector3& result);

	void loadData(const char* filename);
	void saveData(const char* filename);
	void updateMeshes();

	void recalculateMaterials(PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >* mesh, const PolyVox::Vector3DFloat& meshOffset, PolyVox::RawVolume<VoxelType>* volume);
	VoxelType getInterpolatedValue(PolyVox::RawVolume<VoxelType>* volume, const PolyVox::Vector3DFloat& position);

protected:
	Volume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth);
	~Volume();
	Volume& operator=(const Volume&);

	VolumeType mType;

public:
	PolyVox::RawVolume<VoxelType>* mVolData;
	gameplay::Node* mRootNode;
	//VolumeRegion* mVolumeRegion;
	PolyVox::Array<3, VolumeRegion*> mVolumeRegions;
	std::string mMaterialPath;

	unsigned int mRegionWidth;
	unsigned int mRegionHeight;
	unsigned int mRegionDepth;
};

#include "Volume.inl"

#endif //VOLUME_H_
