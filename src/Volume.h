#ifndef VOLUME_H_
#define VOLUME_H_

#include "VolumeRegion.h"

#include "Node.h"
#include "Ref.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"
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
	typedef VoxelType VoxelType;

	gameplay::Node* getRootNode();
	VolumeType getType(void) const;

	VoxelType getVoxelAt(int x, int y, int z);
	void setVoxelAt(int x, int y, int z, VoxelType value);

	bool raycast(gameplay::Ray ray, float distance, gameplay::Vector3& result);

	void loadData(const char* filename);
	void saveData(const char* filename);
	void updateMeshes();

protected:
	Volume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth);
	~Volume();
	Volume& operator=(const Volume&);

	VolumeType mType;

public:
	PolyVox::SimpleVolume<VoxelType>* mVolData;
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
