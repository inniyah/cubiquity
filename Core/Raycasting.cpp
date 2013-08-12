#include "Raycasting.h"
#include "ColouredCubesVolume.h"

#include "PolyVoxCore/Picking.h"

namespace Cubiquity
{
	bool pickFirstSolidVoxel(ColouredCubesVolume* colouredCubesVolume, float startX, float startY, float startZ, float dirAndLengthX, float dirAndLengthY, float dirAndLengthZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ)
	{
		::PolyVox::Vector3DFloat start(startX, startY, startZ);
		::PolyVox::Vector3DFloat dirAndLength(dirAndLengthX, dirAndLengthY, dirAndLengthZ);

		ColouredCubesVolumeImpl* volumeImpl = dynamic_cast<ColouredCubesVolumeImpl*>(colouredCubesVolume);

		::PolyVox::PickResult result = ::PolyVox::pickVoxel(volumeImpl->_getPolyVoxVolume(), start, dirAndLength, Colour(0, 0, 0, 0));

		if(result.didHit)
		{
			*resultX = result.hitVoxel.getX();
			*resultY = result.hitVoxel.getY();
			*resultZ = result.hitVoxel.getZ();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool pickLastEmptyVoxel(ColouredCubesVolume* colouredCubesVolume, float startX, float startY, float startZ, float dirAndLengthX, float dirAndLengthY, float dirAndLengthZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ)
	{
		::PolyVox::Vector3DFloat start(startX, startY, startZ);
		::PolyVox::Vector3DFloat dirAndLength(dirAndLengthX, dirAndLengthY, dirAndLengthZ);

		ColouredCubesVolumeImpl* volumeImpl = dynamic_cast<ColouredCubesVolumeImpl*>(colouredCubesVolume);

		::PolyVox::PickResult result = ::PolyVox::pickVoxel(volumeImpl->_getPolyVoxVolume(), start, dirAndLength, Colour(0, 0, 0, 0));

		if(result.didHit)
		{
			*resultX = result.previousVoxel.getX();
			*resultY = result.previousVoxel.getY();
			*resultZ = result.previousVoxel.getZ();
			return true;
		}
		else
		{
			return false;
		}
	}
}