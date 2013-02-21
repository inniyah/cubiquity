#ifndef CUBIQUITYRAYCASTING_H_
#define CUBIQUITYRAYCASTING_H_

#include "PolyVoxCore/Raycast.h"

template <typename VoxelType>
class RaycastTestFunctor
{
public:
	RaycastTestFunctor()
	{
	}

	bool operator()(PolyVox::Vector3DFloat pos, const VoxelType& voxel)
	{
	}
};

template <>
class RaycastTestFunctor<MultiMaterial4>
{
public:
	RaycastTestFunctor()
	{
	}

	bool operator()(PolyVox::Vector3DFloat pos, const MultiMaterial4& voxel)
	{
		mLastPos = pos;
		return voxel.getSumOfMaterials() <= MultiMaterial4::getMaxMaterialValue() / 2;
	}

	PolyVox::Vector3DFloat mLastPos;
};

template <>
class RaycastTestFunctor<Colour>
{
public:
	RaycastTestFunctor()
	{
	}

	bool operator()(PolyVox::Vector3DFloat pos, const Colour& voxel)
	{
		return false;
	}

	PolyVox::Vector3DFloat mLastPos;
};

class ColouredCubesRaycastTestFunctor
{
public:
	ColouredCubesRaycastTestFunctor()
	{
	}

	bool operator()(const PolyVox::SimpleVolume<Colour>::Sampler& sampler)
	{
		mLastPos = sampler.getPosition();

		return sampler.getVoxel().getAlpha() == 0;
	}

	PolyVox::Vector3DInt32 mLastPos;
};


// Note: This function is not implemented in a very efficient manner and it rather slow.
// A better implementation should make use of the 'peek' functions to sample the voxel data,
// but this will require careful handling of the cases when the ray is outside the volume.
// It could also compute entry and exit points to avoid having to test every step for whether
// it is still inside the volume.
// Also, should we handle computing the exact intersection point? Repeatedly bisect the last
// two points, of perform interpolation between them? Maybe user code could perform such interpolation?
template<typename VolumeType, typename Callback>
PolyVox::RaycastResult smoothRaycastWithDirection(VolumeType* polyVoxVolume, const PolyVox::Vector3DFloat& v3dStart, const PolyVox::Vector3DFloat& v3dDirectionAndLength, Callback& callback, float fStepSize = 1.0f)
{		
	int mMaxNoOfSteps = v3dDirectionAndLength.length() / fStepSize;

	PolyVox::Vector3DFloat v3dPos = v3dStart;
	const PolyVox::Vector3DFloat v3dStep =  v3dDirectionAndLength / static_cast<float>(mMaxNoOfSteps);

	for(uint32_t ct = 0; ct < mMaxNoOfSteps; ct++)
	{
		float fPosX = v3dPos.getX();
		float fPosY = v3dPos.getY();
		float fPosZ = v3dPos.getZ();

		float fFloorX = floor(fPosX);
		float fFloorY = floor(fPosY);
		float fFloorZ = floor(fPosZ);

		float fInterpX = fPosX - fFloorX;
		float fInterpY = fPosY - fFloorY;
		float fInterpZ = fPosZ - fFloorZ;

		// Conditional logic required to round negative floats correctly
		int32_t iX = static_cast<int32_t>(fFloorX > 0.0f ? fFloorX + 0.5f : fFloorX - 0.5f); 
		int32_t iY = static_cast<int32_t>(fFloorY > 0.0f ? fFloorY + 0.5f : fFloorY - 0.5f); 
		int32_t iZ = static_cast<int32_t>(fFloorZ > 0.0f ? fFloorZ + 0.5f : fFloorZ - 0.5f);

		const typename VolumeType::VoxelType& voxel000 = polyVoxVolume->getVoxelAt(iX, iY, iZ);
		const typename VolumeType::VoxelType& voxel001 = polyVoxVolume->getVoxelAt(iX, iY, iZ + 1);
		const typename VolumeType::VoxelType& voxel010 = polyVoxVolume->getVoxelAt(iX, iY + 1, iZ);
		const typename VolumeType::VoxelType& voxel011 = polyVoxVolume->getVoxelAt(iX, iY + 1, iZ + 1);
		const typename VolumeType::VoxelType& voxel100 = polyVoxVolume->getVoxelAt(iX + 1, iY, iZ);
		const typename VolumeType::VoxelType& voxel101 = polyVoxVolume->getVoxelAt(iX + 1, iY, iZ + 1);
		const typename VolumeType::VoxelType& voxel110 = polyVoxVolume->getVoxelAt(iX + 1, iY + 1, iZ);
		const typename VolumeType::VoxelType& voxel111 = polyVoxVolume->getVoxelAt(iX + 1, iY + 1, iZ + 1);

		typename VolumeType::VoxelType tInterpolatedValue = PolyVox::trilerp(voxel000,voxel100,voxel010,voxel110,voxel001,voxel101,voxel011,voxel111,fInterpX,fInterpY,fInterpZ);
		
		if(!callback(v3dPos, tInterpolatedValue))
		{
			return PolyVox::RaycastResults::Interupted;
		}

		v3dPos += v3dStep;
	}

	return PolyVox::RaycastResults::Completed;
}

#endif //CUBIQUITYRAYCASTING_H_