#include "SmoothTerrainVolumeSculpting.h"

#include "SmoothTerrainVolume.h"

using namespace PolyVox;

namespace Cubiquity
{
	void sculptSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float radius)
	{
		int firstX = static_cast<int>(std::floor(centre.getX() - radius));
		int firstY = static_cast<int>(std::floor(centre.getY() - radius));
		int firstZ = static_cast<int>(std::floor(centre.getZ() - radius));

		int lastX = static_cast<int>(std::ceil(centre.getX() + radius));
		int lastY = static_cast<int>(std::ceil(centre.getY() + radius));
		int lastZ = static_cast<int>(std::ceil(centre.getZ() + radius));

		float radiusSquared = radius * radius;

		//Check bounds.
		firstX = (std::max)(firstX,smoothTerrainVolume->getEnclosingRegion().getLowerCorner().getX());
		firstY = (std::max)(firstY,smoothTerrainVolume->getEnclosingRegion().getLowerCorner().getY());
		firstZ = (std::max)(firstZ,smoothTerrainVolume->getEnclosingRegion().getLowerCorner().getZ());

		lastX = (std::min)(lastX,smoothTerrainVolume->getEnclosingRegion().getUpperCorner().getX());
		lastY = (std::min)(lastY,smoothTerrainVolume->getEnclosingRegion().getUpperCorner().getY());
		lastZ = (std::min)(lastZ,smoothTerrainVolume->getEnclosingRegion().getUpperCorner().getZ());

		Region region(firstX, firstY, firstZ, lastX, lastY, lastZ);

		::PolyVox::RawVolume<MultiMaterial> mSmoothingVolume(region);

		for(int z = firstZ; z <= lastZ; ++z)
		{
			for(int y = firstY; y <= lastY; ++y)
			{
				for(int x = firstX; x <= lastX; ++x)
				{					
					Vector3F pos(x, y, z);
					float distFromCentre = (centre - pos).length(); //Should use squared length?
					float invDistFromCenter = radius - distFromCentre;
					if(invDistFromCenter < 0.0f)
					{
						invDistFromCenter = 0.0f;
					}

					invDistFromCenter *= invDistFromCenter;

					int32_t amountToAdd = static_cast<int32_t>(invDistFromCenter + 0.5f);

					for(uint32_t matIndex = 0; matIndex < MultiMaterial::getNoOfMaterials(); matIndex++)
					{
						uint32_t original = smoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex);

						if(amountToAdd > 0)
						{
							uint32_t sum = 0;
							sum += smoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex);
							sum += smoothTerrainVolume->getVoxelAt(x+1, y, z).getMaterial(matIndex);
							sum += smoothTerrainVolume->getVoxelAt(x-1, y, z).getMaterial(matIndex);
							sum += smoothTerrainVolume->getVoxelAt(x, y+1, z).getMaterial(matIndex);
							sum += smoothTerrainVolume->getVoxelAt(x, y-1, z).getMaterial(matIndex);
							sum += smoothTerrainVolume->getVoxelAt(x, y, z+1).getMaterial(matIndex);
							sum += smoothTerrainVolume->getVoxelAt(x, y, z-1).getMaterial(matIndex);

							uint32_t average = sum / 7;
							uint32_t rem = sum % 7;
							if(rem > 3)
							{
								average++;
							}

							average += amountToAdd;
							average = (std::min)(average, MultiMaterial::getMaxMaterialValue());
							average = (std::max)(average, original); // For some reason matieral gets slightly eroded unless we use this.

							MultiMaterial result = mSmoothingVolume.getVoxelAt(x, y, z);
							result.setMaterial(matIndex, average);
							mSmoothingVolume.setVoxel(x, y, z, result);
						}
						else
						{
							MultiMaterial result = mSmoothingVolume.getVoxelAt(x, y, z);
							result.setMaterial(matIndex, original);
							mSmoothingVolume.setVoxel(x, y, z, result);
						}
					}

					MultiMaterial result = mSmoothingVolume.getVoxelAt(x, y, z);
					result.clampSumOfMaterials();
					mSmoothingVolume.setVoxel(x, y, z, result);
				}
			}
		}

		for(int z = firstZ; z <= lastZ; ++z)
		{
			for(int y = firstY; y <= lastY; ++y)
			{
				for(int x = firstX; x <= lastX; ++x)
				{
					smoothTerrainVolume->setVoxelAt(x, y, z, mSmoothingVolume.getVoxelAt(x, y, z), UpdatePriorities::DontUpdate);
				}
			}
		}

		smoothTerrainVolume->markAsModified(region, UpdatePriorities::Immediate);
	}
}
