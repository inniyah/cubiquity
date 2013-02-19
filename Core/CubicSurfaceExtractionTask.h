#ifndef CUBIQUITY_CUBICSURFACEEXTRACTIONTASK_H_
#define CUBIQUITY_CUBICSURFACEEXTRACTIONTASK_H_

#include "PolyVoxCore/CubicSurfaceExtractor.h"

#include "Colour.h"

template<typename VolumeType, typename IsQuadNeeded>
class CubicSurfaceExtractionTask
{
public:
	CubicSurfaceExtractionTask(VolumeType* volData, PolyVox::Region region, PolyVox::SurfaceMesh<PolyVox::PositionMaterial<typename VolumeType::VoxelType> >* result, PolyVox::WrapMode eWrapMode = PolyVox::WrapModes::Border, typename VolumeType::VoxelType tBorderValue = VolumeType::VoxelType(0), bool bMergeQuads = true, IsQuadNeeded isQuadNeeded = IsQuadNeeded());
	~CubicSurfaceExtractionTask();

	void process(void);

public:
	//PolyVox::CubicSurfaceExtractor<VolumeType, IsQuadNeeded>* mCubicSurfaceExtractor;

	VolumeType* mVolData;
	PolyVox::Region mRegion;
	PolyVox::SurfaceMesh<PolyVox::PositionMaterial<typename VolumeType::VoxelType> >* mResult;
	PolyVox::WrapMode mWrapMode;
	typename VolumeType::VoxelType mBorderValue;
	bool mMergeQuads;
	IsQuadNeeded mIsQuadNeeded;
};

#include "CubicSurfaceExtractionTask.inl"

#endif //CUBIQUITY_CUBICSURFACEEXTRACTIONTASK_H_
