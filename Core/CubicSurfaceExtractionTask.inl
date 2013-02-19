template<typename VolumeType, typename IsQuadNeeded>
CubicSurfaceExtractionTask<VolumeType, IsQuadNeeded>::CubicSurfaceExtractionTask(VolumeType* volData, PolyVox::Region region, PolyVox::SurfaceMesh<PolyVox::PositionMaterial<typename VolumeType::VoxelType> >* result, PolyVox::WrapMode eWrapMode, typename VolumeType::VoxelType tBorderValue, bool bMergeQuads, IsQuadNeeded isQuadNeeded)
	:mVolData(volData)
	,mRegion(region)
	,mResult(result)
	,mWrapMode(eWrapMode)
	,mBorderValue(tBorderValue)
	,mMergeQuads(bMergeQuads)
	,mIsQuadNeeded(isQuadNeeded)
{
	//mCubicSurfaceExtractor = new PolyVox::CubicSurfaceExtractor<VolumeType, IsQuadNeeded>(volData, region, result, eWrapMode, tBorderValue, bMergeQuads, isQuadNeeded);
}

template<typename VolumeType, typename IsQuadNeeded>
CubicSurfaceExtractionTask<VolumeType, IsQuadNeeded>::~CubicSurfaceExtractionTask()
{
	//delete mCubicSurfaceExtractor;
}

template<typename VolumeType, typename IsQuadNeeded>
void CubicSurfaceExtractionTask<VolumeType, IsQuadNeeded>::process(void)
{
	PolyVox::CubicSurfaceExtractor<VolumeType, IsQuadNeeded> cubicSurfaceExtractor(mVolData, mRegion, mResult, mWrapMode, mBorderValue, mMergeQuads, mIsQuadNeeded);
	cubicSurfaceExtractor.execute();
}