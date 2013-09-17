#include "SmoothTerrainVolumeEditor.h"

#include "Brush.h"
#include "SmoothSurfaceExtractionTask.h" // FIXME - Shouldn't need this here.
#include "SmoothTerrainVolume.h"

using namespace PolyVox;

namespace Cubiquity
{
	void sculptSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, const Brush& brush)
	{
		// Values for Gaussian function: https://en.wikipedia.org/wiki/Gaussian_function
		float a = brush.opacity();
		//float b = 0.0f;
		float c = brush.outerRadius() / 3.0f; // 3 standard deviations covers most of the range.
		float cc2 = 2.0f*c*c;

		int firstX = static_cast<int>(std::floor(centre.getX() - brush.outerRadius()));
		int firstY = static_cast<int>(std::floor(centre.getY() - brush.outerRadius()));
		int firstZ = static_cast<int>(std::floor(centre.getZ() - brush.outerRadius()));

		int lastX = static_cast<int>(std::ceil(centre.getX() + brush.outerRadius()));
		int lastY = static_cast<int>(std::ceil(centre.getY() + brush.outerRadius()));
		int lastZ = static_cast<int>(std::ceil(centre.getZ() + brush.outerRadius()));

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

						int32_t iAverage = sample;

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

	void blurSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float brushRadius, float amount)
	{
		// Values for Gaussian function: https://en.wikipedia.org/wiki/Gaussian_function
		float a = amount;
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
						sum += smoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x+1, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x-1, y, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y+1, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y-1, z).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y, z+1).getMaterial(matIndex);
						sum += smoothTerrainVolume->getVoxelAt(x, y, z-1).getMaterial(matIndex);

						float fAverage = static_cast<float>(sum) / 7.0f;

						float fLerped = lerp(static_cast<float>(original), fAverage, gaussian);

						int32_t iLerped = static_cast<int32_t>(fLerped + 0.5f);

						// Prevent wrapping around.
						iLerped = (std::min)(iLerped, static_cast<int32_t>(MultiMaterial::getMaxMaterialValue()));
						iLerped = (std::max)(iLerped, 0);

						MultiMaterial result = mSmoothingVolume.getVoxelAt(x, y, z);
						result.setMaterial(matIndex, iLerped);
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

	void addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial& material)
	{
		uint32_t indexToRemoveFrom = 0; //FIXME - start somewhere random
		uint32_t iterationWithNoRemovals = 0;
		while(amountToAdd > 0)
		{
			if(indexToRemoveFrom != index)
			{
				if(material.getMaterial(indexToRemoveFrom) > 0)
				{
					material.setMaterial(index, material.getMaterial(index) + 1);
					material.setMaterial(indexToRemoveFrom, material.getMaterial(indexToRemoveFrom) - 1);
					amountToAdd--;
					iterationWithNoRemovals = 0;
				}
				else
				{
					iterationWithNoRemovals++;
				}
			}
			else
			{
				iterationWithNoRemovals++;
			}

			if(iterationWithNoRemovals == MultiMaterial::getNoOfMaterials())
			{
				break;
			}

			indexToRemoveFrom++;
			indexToRemoveFrom %= MultiMaterial::getNoOfMaterials();
		}
	}

	void paintSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float brushRadius, uint32_t materialIndex, float amount)
	{
		// Values for Gaussian function: https://en.wikipedia.org/wiki/Gaussian_function
		float a = amount;
		//float b = 0.0f;
		float c = brushRadius / 3.0f; // 3 standard deviations covers most of the range.
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

		//::PolyVox::RawVolume<MultiMaterial> mSmoothingVolume(region);

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

					float fAmmountToAdd = gaussian * MultiMaterial::getMaxMaterialValue();

					int32_t amountToAdd = static_cast<int32_t>(fAmmountToAdd + 0.5f);


					MultiMaterial voxel = smoothTerrainVolume->getVoxelAt(x, y, z);
					addToMaterial(materialIndex, amountToAdd, voxel);
					voxel.clampSumOfMaterials();
					smoothTerrainVolume->setVoxelAt(x, y, z, voxel);
					
				}
			}
		}

		smoothTerrainVolume->markAsModified(region, UpdatePriorities::Immediate);
	}
}