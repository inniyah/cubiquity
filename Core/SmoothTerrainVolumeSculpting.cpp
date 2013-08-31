#include "SmoothTerrainVolumeSculpting.h"

#include "SmoothTerrainVolume.h"

using namespace PolyVox;

namespace Cubiquity
{
	void sculptSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float radius)
	{
		float height = -5.0f;
		float standardDeviation = 3.0f;

		// Values for Gaussian function: https://en.wikipedia.org/wiki/Gaussian_function
		float a = height;
		//float b = 0.0f;
		float c = standardDeviation;
		float cc2 = 2.0f*c*c;

		radius = c * 3.0f;

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
					float distFromCentreSquared = (centre - pos).lengthSquared();


					// From Wikipedia: https://en.wikipedia.org/wiki/Gaussian_function
					// Gaussian funtion f(x) = a*exp(-((x-b)*(x-b) / (2*c*c)))
					// For us, 'b' is zero, which leaves 'x*x' on top. 'x' is the distance
					// from the centre so we can put the squared distance on top.
					float gaussian = a*exp(-(distFromCentreSquared / cc2));

					int32_t amountToAdd = static_cast<int32_t>(gaussian + 0.5f);

					for(uint32_t matIndex = 0; matIndex < MultiMaterial::getNoOfMaterials(); matIndex++)
					{
						int32_t original = smoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex);

						int32_t sum = 0;
						sum += smoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x+1, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x-1, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y+1, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y-1, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y, z+1).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y, z-1).getMaterial(matIndex);

						int32_t average = sum / 7;
						int32_t rem = sum % 7;
						if(rem > 3)
						{
							average++;
						}

						//average += amountToAdd;

						

						if(height >= 0.0f)
						{
							average = (std::max)(average, original); // For some reason matieral gets slightly eroded unless we use this.
						}
						if(height <= 0.0f)
						{
							average++;
							average++;
							average++;
							average = (std::min)(average, original); // For some reason matieral gets slightly dilated unless we use this.
						}

						// Prevent wrapping around.
						average = (std::min)(average, static_cast<int32_t>(MultiMaterial::getMaxMaterialValue()));
						average = (std::max)(average, 0);

						MultiMaterial result = mSmoothingVolume.getVoxelAt(x, y, z);
						result.setMaterial(matIndex, average);
						mSmoothingVolume.setVoxel(x, y, z, result);
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
