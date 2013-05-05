#ifndef CUBIQUITYRAYCASTING_H_
#define CUBIQUITYRAYCASTING_H_

#include "Colour.h"
#include "MultiMaterial.h"

#include "PolyVoxCore/Raycast.h"

namespace Cubiquity
{
	template <typename VoxelType>
	class RaycastTestFunctor
	{
	public:
		RaycastTestFunctor()
		{
		}

		bool operator()(Vector3F pos, const VoxelType& voxel)
		{
		}
	};

	template <>
	class RaycastTestFunctor<MultiMaterial>
	{
	public:
		RaycastTestFunctor()
		{
		}

		bool operator()(Vector3F pos, const MultiMaterial& voxel)
		{
			mLastPos = pos;
			return voxel.getSumOfMaterials() <= MultiMaterial::getMaxMaterialValue() / 2;
		}

		Vector3F mLastPos;
	};

	template <>
	class RaycastTestFunctor<Colour>
	{
	public:
		RaycastTestFunctor()
		{
		}

		bool operator()(Vector3F pos, const Colour& voxel)
		{
			return false;
		}

		Vector3F mLastPos;
	};

	class ColouredCubesRaycastTestFunctor
	{
	public:
		ColouredCubesRaycastTestFunctor()
		{
		}

		bool operator()(const ::PolyVox::SimpleVolume<Colour>::Sampler& sampler)
		{
			mLastPos = sampler.getPosition();

			return sampler.getVoxel().getAlpha() == 0;
		}

		Vector3I mLastPos;
	};


	// Note: This function is not implemented in a very efficient manner and it rather slow.
	// A better implementation should make use of the 'peek' functions to sample the voxel data,
	// but this will require careful handling of the cases when the ray is outside the volume.
	// It could also compute entry and exit points to avoid having to test every step for whether
	// it is still inside the volume.
	// Also, should we handle computing the exact intersection point? Repeatedly bisect the last
	// two points, of perform interpolation between them? Maybe user code could perform such interpolation?
	template<typename PolyVoxVolumeType, typename Callback>
	::PolyVox::RaycastResult smoothRaycastWithDirection(PolyVoxVolumeType* polyVoxVolume, const Vector3F& v3dStart, const Vector3F& v3dDirectionAndLength, Callback& callback, float fStepSize = 1.0f)
	{		
		POLYVOX_ASSERT(fStepSize > 0.0f, "Raycast step size must be greater than zero");
		uint32_t mMaxNoOfSteps = static_cast<uint32_t>(v3dDirectionAndLength.length() / fStepSize);

		Vector3F v3dPos = v3dStart;
		const Vector3F v3dStep =  v3dDirectionAndLength / static_cast<float>(mMaxNoOfSteps);

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

			const typename PolyVoxVolumeType::VoxelType& voxel000 = polyVoxVolume->getVoxelAt(iX, iY, iZ);
			const typename PolyVoxVolumeType::VoxelType& voxel001 = polyVoxVolume->getVoxelAt(iX, iY, iZ + 1);
			const typename PolyVoxVolumeType::VoxelType& voxel010 = polyVoxVolume->getVoxelAt(iX, iY + 1, iZ);
			const typename PolyVoxVolumeType::VoxelType& voxel011 = polyVoxVolume->getVoxelAt(iX, iY + 1, iZ + 1);
			const typename PolyVoxVolumeType::VoxelType& voxel100 = polyVoxVolume->getVoxelAt(iX + 1, iY, iZ);
			const typename PolyVoxVolumeType::VoxelType& voxel101 = polyVoxVolume->getVoxelAt(iX + 1, iY, iZ + 1);
			const typename PolyVoxVolumeType::VoxelType& voxel110 = polyVoxVolume->getVoxelAt(iX + 1, iY + 1, iZ);
			const typename PolyVoxVolumeType::VoxelType& voxel111 = polyVoxVolume->getVoxelAt(iX + 1, iY + 1, iZ + 1);

			typename PolyVoxVolumeType::VoxelType tInterpolatedValue = ::PolyVox::trilerp(voxel000,voxel100,voxel010,voxel110,voxel001,voxel101,voxel011,voxel111,fInterpX,fInterpY,fInterpZ);
		
			if(!callback(v3dPos, tInterpolatedValue))
			{
				return ::PolyVox::RaycastResults::Interupted;
			}

			v3dPos += v3dStep;
		}

		return ::PolyVox::RaycastResults::Completed;
	}
}

#endif //CUBIQUITYRAYCASTING_H_