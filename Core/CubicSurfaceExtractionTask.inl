template<typename VolumeType, typename IsQuadNeeded>
CubicSurfaceExtractionTask<VolumeType, IsQuadNeeded>::CubicSurfaceExtractionTask(VolumeType* volData, PolyVox::Region region, PolyVox::SurfaceMesh<PolyVox::PositionMaterial<typename VolumeType::VoxelType> >* result, PolyVox::WrapMode eWrapMode, typename VolumeType::VoxelType tBorderValue, bool bMergeQuads, IsQuadNeeded isQuadNeeded, uint32_t downSampleFactor)
	:mVolData(volData)
	,mRegion(region)
	,mResult(result)
	,mWrapMode(eWrapMode)
	,mBorderValue(tBorderValue)
	,mMergeQuads(bMergeQuads)
	,mIsQuadNeeded(isQuadNeeded)
	,mDownSampleFactor(downSampleFactor)
{
}

template<typename VolumeType, typename IsQuadNeeded>
CubicSurfaceExtractionTask<VolumeType, IsQuadNeeded>::~CubicSurfaceExtractionTask()
{
}

template<typename VolumeType, typename IsQuadNeeded>
void CubicSurfaceExtractionTask<VolumeType, IsQuadNeeded>::process(void)
{
	PolyVox::CubicSurfaceExtractor<VolumeType, IsQuadNeeded> cubicSurfaceExtractor(mVolData, mRegion, mResult, mWrapMode, mBorderValue, mMergeQuads, mIsQuadNeeded);
	cubicSurfaceExtractor.execute();

	if(mDownSampleFactor == 2)
	{
		mResult->scaleVertices(mDownSampleFactor);
		mResult->translateVertices(Vector3DFloat(0.5f, 0.5f, 0.5f));
	}
	else if(mDownSampleFactor == 4)
	{
		mResult->scaleVertices(mDownSampleFactor);
		mResult->translateVertices(Vector3DFloat(1.5f, 1.5f, 1.5f));
	}
}