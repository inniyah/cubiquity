#include "SmoothTerrainVolumeSculpting.h"

#include "SmoothSurfaceExtractionTask.h" // FIXME - Shouldn't need this here.
#include "SmoothTerrainVolume.h"

using namespace PolyVox;

namespace Cubiquity
{
	void sculptSmoothTerrainVolumeByBlurAndMultiply(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float brushRadius, float speed)
	{
		// Values for Gaussian function: https://en.wikipedia.org/wiki/Gaussian_function
		float a = speed;
		//float b = 0.0f;
		float c = brushRadius / 3.0f;
		float cc2 = 2.0f*c*c;

		int firstX = static_cast<int>(std::floor(centre.getX() - brushRadius));
		int firstY = static_cast<int>(std::floor(centre.getY() - brushRadius));
		int firstZ = static_cast<int>(std::floor(centre.getZ() - brushRadius));

		int lastX = static_cast<int>(std::ceil(centre.getX() + brushRadius));
		int lastY = static_cast<int>(std::ceil(centre.getY() + brushRadius));
		int lastZ = static_cast<int>(std::ceil(centre.getZ() + brushRadius));

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
						sum += (smoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex) * 16);
						sum += smoothTerrainVolume->getVoxelAt(x+1, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x-1, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y+1, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y-1, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y, z+1).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y, z-1).getMaterial(matIndex);

						float fAverage = static_cast<float>(sum) / 22.0f;

						fAverage *= (1.0f + gaussian);
						//fAverage += gaussian;

						int32_t iAverage = static_cast<int32_t>(fAverage + 0.5f);

						

						/*if(speed >= 0.0f)
						{
							iAverage = (std::max)(iAverage, original); // For some reason material gets slightly eroded unless we use this.
						}
						if(speed <= 0.0f)
						{
							iAverage = (std::min)(iAverage, original); // For some reason material gets slightly dilated unless we use this.
						}*/

						// Prevent wrapping around.
						iAverage = (std::min)(iAverage, static_cast<int32_t>(MultiMaterial::getMaxMaterialValue()));
						iAverage = (std::max)(iAverage, 1);

						MultiMaterial result = mSmoothingVolume.getVoxelAt(x, y, z);
						result.setMaterial(matIndex, iAverage);
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

	void sculptSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float brushRadius, float speed)
	{
		// Values for Gaussian function: https://en.wikipedia.org/wiki/Gaussian_function
		float a = speed;
		//float b = 0.0f;
		float c = brushRadius / 3.0f;
		float cc2 = 2.0f*c*c;

		int firstX = static_cast<int>(std::floor(centre.getX() - brushRadius));
		int firstY = static_cast<int>(std::floor(centre.getY() - brushRadius));
		int firstZ = static_cast<int>(std::floor(centre.getZ() - brushRadius));

		int lastX = static_cast<int>(std::ceil(centre.getX() + brushRadius));
		int lastY = static_cast<int>(std::ceil(centre.getY() + brushRadius));
		int lastZ = static_cast<int>(std::ceil(centre.getZ() + brushRadius));

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

					/*Vector3F vecFromCentre = pos - centre;
					float oldLength = vecFromCentre.length();
					if(oldLength > 0.0001f)
					{
						float newLength = max(oldLength - 1.0f, 0.0f);
						vecFromCentre /= oldLength;
						vecFromCentre *= newLength;
					}*/	

					//Vector3F samplePoint = centre + vecFromCentre;

					//MultiMaterial sample = getInterpolatedValue(smoothTerrainVolume->_getPolyVoxVolume(), samplePoint);


					// From Wikipedia: https://en.wikipedia.org/wiki/Gaussian_function
					// Gaussian funtion f(x) = a*exp(-((x-b)*(x-b) / (2*c*c)))
					// For us, 'b' is zero, which leaves 'x*x' on top. 'x' is the distance
					// from the centre so we can put the squared distance on top.
					float gaussian = a*exp(-(distFromCentreSquared / cc2));

					int32_t amountToAdd = static_cast<int32_t>(gaussian + 0.5f);

					for(uint32_t matIndex = 0; matIndex < MultiMaterial::getNoOfMaterials(); matIndex++)
					{
						int32_t original = smoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex);

						float voxel1nx = static_cast<float>(smoothTerrainVolume->getVoxelAt(x-1, y, z).getMaterial(matIndex));
						float voxel1px = static_cast<float>(smoothTerrainVolume->getVoxelAt(x+1, y, z).getMaterial(matIndex));

						float voxel1ny = static_cast<float>(smoothTerrainVolume->getVoxelAt(x, y-1, z).getMaterial(matIndex));
						float voxel1py = static_cast<float>(smoothTerrainVolume->getVoxelAt(x, y+1, z).getMaterial(matIndex));

						float voxel1nz = static_cast<float>(smoothTerrainVolume->getVoxelAt(x, y, z-1).getMaterial(matIndex));
						float voxel1pz = static_cast<float>(smoothTerrainVolume->getVoxelAt(x, y, z+1).getMaterial(matIndex));

						Vector3DFloat normal
						(
							voxel1nx - voxel1px,
							voxel1ny - voxel1py,
							voxel1nz - voxel1pz
						);

						if(normal.length() > 0.001)
						{
							normal.normalise();
						}

						normal = normal * gaussian;

						Vector3F samplePoint = pos - normal;

						int32_t sample = getInterpolatedValue(smoothTerrainVolume->_getPolyVoxVolume(), samplePoint).getMaterial(matIndex);

						/*int32_t sum = 0;
						sum += (smoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex) * 16);*/
						/*sum += smoothTerrainVolume->getVoxelAt(x+1, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x-1, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y+1, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y-1, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y, z+1).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y, z-1).getMaterial(matIndex);

						float fAverage = static_cast<float>(sum) / 22.0f;

						fAverage *= (1.0f + gaussian);
						//fAverage += gaussian;

						int32_t iAverage = static_cast<int32_t>(fAverage + 0.5f);*/

						

						int32_t iAverage = (std::max)(original, sample);

						//iAverage = lerp(original, iAverage, gaussian);

						

						/*if(speed >= 0.0f)
						{
							iAverage = (std::max)(iAverage, original); // For some reason material gets slightly eroded unless we use this.
						}
						if(speed <= 0.0f)
						{
							iAverage = (std::min)(iAverage, original); // For some reason material gets slightly dilated unless we use this.
						}*/

						// Prevent wrapping around.
						iAverage = (std::min)(iAverage, static_cast<int32_t>(MultiMaterial::getMaxMaterialValue()));
						iAverage = (std::max)(iAverage, 1);

						MultiMaterial result = mSmoothingVolume.getVoxelAt(x, y, z);
						result.setMaterial(matIndex, iAverage);
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
