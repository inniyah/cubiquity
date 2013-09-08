#include "SmoothTerrainVolumeEditor.h"

#include "SmoothSurfaceExtractionTask.h" // FIXME - Shouldn't need this here.
#include "SmoothTerrainVolume.h"

using namespace PolyVox;

namespace Cubiquity
{
	SmoothTerrainVolumeEditor::SmoothTerrainVolumeEditor(SmoothTerrainVolumeImpl* volume)
		:mSmoothTerrainVolume(volume)
		,mSmoothingVolume(0)
	{
	}

	SmoothTerrainVolumeEditor::~SmoothTerrainVolumeEditor()
	{
		if(mSmoothingVolume)
		{
			delete mSmoothingVolume;
			mSmoothingVolume = 0;
		}
	}

	void SmoothTerrainVolumeEditor::edit(const Vector3F& centre, float radius, uint32_t materialToUse, EditAction editAction, float timeElapsedInSeconds, float amount, float smoothBias)
	{
		// We'll set this flag for operations which actually modify the shape of the
		// mesh, as these can cause temporary cracks if not processed immediately.
		bool needsImmediateUpdate = false;

		int firstX = static_cast<int>(std::floor(centre.getX() - radius));
		int firstY = static_cast<int>(std::floor(centre.getY() - radius));
		int firstZ = static_cast<int>(std::floor(centre.getZ() - radius));

		int lastX = static_cast<int>(std::ceil(centre.getX() + radius));
		int lastY = static_cast<int>(std::ceil(centre.getY() + radius));
		int lastZ = static_cast<int>(std::ceil(centre.getZ() + radius));

		float radiusSquared = radius * radius;

		//Check bounds.
		firstX = (std::max)(firstX,mSmoothTerrainVolume->getEnclosingRegion().getLowerCorner().getX());
		firstY = (std::max)(firstY,mSmoothTerrainVolume->getEnclosingRegion().getLowerCorner().getY());
		firstZ = (std::max)(firstZ,mSmoothTerrainVolume->getEnclosingRegion().getLowerCorner().getZ());

		lastX = (std::min)(lastX,mSmoothTerrainVolume->getEnclosingRegion().getUpperCorner().getX());
		lastY = (std::min)(lastY,mSmoothTerrainVolume->getEnclosingRegion().getUpperCorner().getY());
		lastZ = (std::min)(lastZ,mSmoothTerrainVolume->getEnclosingRegion().getUpperCorner().getZ());

		Region region(firstX, firstY, firstZ, lastX, lastY, lastZ);

		// Do the smoothing once, before we start iterating over all the voxels.
		// At a per-voxel level we only need to do the interpolation.
		if(editAction == EditActions::Smooth)
		{
			// Make sure our temporary target exists, and create it if not.
			if(!mSmoothingVolume)
			{
				mSmoothingVolume = new ::PolyVox::RawVolume<MultiMaterial>(region);
			}
			else
			{
				// If if does exist then make sure it's the right size.
				if(mSmoothingVolume->getEnclosingRegion() != region)
				{
					delete mSmoothingVolume;
					mSmoothingVolume = new ::PolyVox::RawVolume<MultiMaterial>(region);
				}
			}

			// We might not need to do this at float precision, it should be tested again.
			::PolyVox::LowPassFilter< ::PolyVox::POLYVOX_VOLUME<MultiMaterial>, ::PolyVox::RawVolume<MultiMaterial>, ::PolyVox::Vector<4, float> > lowPassFilter(mSmoothTerrainVolume->_getPolyVoxVolume(), region, mSmoothingVolume, region, 3);
			lowPassFilter.execute();
		}

		for(int z = firstZ; z <= lastZ; ++z)
		{
			for(int y = firstY; y <= lastY; ++y)
			{
				for(int x = firstX; x <= lastX; ++x)
				{
					Vector3DFloat currentPos(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
					if((centre - currentPos).lengthSquared() <= radiusSquared)
					{
						float falloff = falloff = (centre - currentPos).length() / radius;
						falloff = (std::min)((std::max)(falloff, 0.0f), 1.0f);
						falloff = 1.0f - falloff;

						// Falloff will be one for most of the sphere and will
						// then rapidly drop to zero over the last few voxels.
						falloff *= 0.3f;
						falloff += 0.7f;

						float amountToEditBy = falloff * timeElapsedInSeconds * amount * static_cast<float>(MultiMaterial::getMaxMaterialValue());

						// The logic for smoothing is rather different from adding/subtracting/painting
						// so the 'amountToEditBy' doesn't apply in the same way. The multiplier below
						// just makes it behave sensibly.
						if(editAction == EditActions::Smooth)
						{
							amountToEditBy *= 0.001f;
						}
						else
						{
							amountToEditBy *= 0.01f;
						}

						uint8_t uToAddOrSubtract = static_cast<uint8_t>(amountToEditBy + 0.5f);

						switch(editAction)
						{
						case EditActions::Add:
							{		
								MultiMaterial originalMat = mSmoothTerrainVolume->getVoxelAt(x, y, z);	
								uint32_t sumOfMaterials = originalMat.getSumOfMaterials();
								if(sumOfMaterials + uToAddOrSubtract <= originalMat.getMaxMaterialValue())
								{
									originalMat.setMaterial(materialToUse, originalMat.getMaterial(materialToUse) + uToAddOrSubtract);
								}
								else
								{
									addToMaterial(materialToUse, uToAddOrSubtract, originalMat);
								}
								mSmoothTerrainVolume->setVoxelAt(x,y,z, originalMat, UpdatePriorities::DontUpdate);
								needsImmediateUpdate = true;
								break;
							}
						case EditActions::Subtract:
							{
								MultiMaterial originalMat = mSmoothTerrainVolume->getVoxelAt(x, y, z);	
								uint32_t sumOfMaterials = originalMat.getSumOfMaterials();
								subtractFromMaterial(uToAddOrSubtract, originalMat);
								mSmoothTerrainVolume->setVoxelAt(x,y,z, originalMat, UpdatePriorities::DontUpdate);
								needsImmediateUpdate = true;
								break;
							}
						case EditActions::Paint:
							{						
								MultiMaterial originalMat = mSmoothTerrainVolume->getVoxelAt(x, y, z);	
								addToMaterial(materialToUse, uToAddOrSubtract, originalMat);
								mSmoothTerrainVolume->setVoxelAt(x,y,z, originalMat, UpdatePriorities::DontUpdate);
								break;
							}
						case EditActions::Smooth:
							{		
								MultiMaterial originalMat = mSmoothTerrainVolume->getVoxelAt(x, y, z);
								MultiMaterial smoothedMat = mSmoothingVolume->getVoxelAt(x, y, z);

								//FIXME - expose linear interpolation as well as trilinear interpolation from PolyVox?
								float orig0 = static_cast<float>(originalMat.getMaterial(0));
								float orig1 = static_cast<float>(originalMat.getMaterial(1));
								float orig2 = static_cast<float>(originalMat.getMaterial(2));
								float orig3 = static_cast<float>(originalMat.getMaterial(3));

								float smooth0 = static_cast<float>(smoothedMat.getMaterial(0));
								float smooth1 = static_cast<float>(smoothedMat.getMaterial(1));
								float smooth2 = static_cast<float>(smoothedMat.getMaterial(2));
								float smooth3 = static_cast<float>(smoothedMat.getMaterial(3));

								float interp0 = (smooth0 - orig0) * amountToEditBy + orig0;
								float interp1 = (smooth1 - orig1) * amountToEditBy + orig1;
								float interp2 = (smooth2 - orig2) * amountToEditBy + orig2;
								float interp3 = (smooth3 - orig3) * amountToEditBy + orig3;

								MultiMaterial interpMat;
								// In theory we should add 0.5f before casting to round
								// properly, but this seems to cause material to grow too much.
								// Instead we add a user-supplied bias value.
								float bias = smoothBias;
								interpMat.setMaterial(0, (std::max<uint32_t>)(0, (std::min)(originalMat.getMaxMaterialValue(), static_cast<uint32_t>(interp0 + bias))));
								interpMat.setMaterial(1, (std::max<uint32_t>)(0, (std::min)(originalMat.getMaxMaterialValue(), static_cast<uint32_t>(interp1 + bias))));
								interpMat.setMaterial(2, (std::max<uint32_t>)(0, (std::min)(originalMat.getMaxMaterialValue(), static_cast<uint32_t>(interp2 + bias))));
								interpMat.setMaterial(3, (std::max<uint32_t>)(0, (std::min)(originalMat.getMaxMaterialValue(), static_cast<uint32_t>(interp3 + bias))));

								mSmoothTerrainVolume->setVoxelAt(x,y,z, interpMat, UpdatePriorities::DontUpdate);
								needsImmediateUpdate = true;

								break;
							}
						}
					}
				}
			}
		}

		mSmoothTerrainVolume->markAsModified(region, needsImmediateUpdate ? UpdatePriorities::Immediate : UpdatePriorities::Background);
	}

	void SmoothTerrainVolumeEditor::smoothVolume(const Vector3F& centre, float radius)
	{
		int firstX = static_cast<int>(std::floor(centre.getX() - radius));
		int firstY = static_cast<int>(std::floor(centre.getY() - radius));
		int firstZ = static_cast<int>(std::floor(centre.getZ() - radius));

		int lastX = static_cast<int>(std::ceil(centre.getX() + radius));
		int lastY = static_cast<int>(std::ceil(centre.getY() + radius));
		int lastZ = static_cast<int>(std::ceil(centre.getZ() + radius));

		float radiusSquared = radius * radius;

		//Check bounds.
		firstX = (std::max)(firstX,mSmoothTerrainVolume->getEnclosingRegion().getLowerCorner().getX());
		firstY = (std::max)(firstY,mSmoothTerrainVolume->getEnclosingRegion().getLowerCorner().getY());
		firstZ = (std::max)(firstZ,mSmoothTerrainVolume->getEnclosingRegion().getLowerCorner().getZ());

		lastX = (std::min)(lastX,mSmoothTerrainVolume->getEnclosingRegion().getUpperCorner().getX());
		lastY = (std::min)(lastY,mSmoothTerrainVolume->getEnclosingRegion().getUpperCorner().getY());
		lastZ = (std::min)(lastZ,mSmoothTerrainVolume->getEnclosingRegion().getUpperCorner().getZ());

		Region region(firstX, firstY, firstZ, lastX, lastY, lastZ);

		// Make sure our temporary target exists, and create it if not.
		if(!mSmoothingVolume)
		{
			mSmoothingVolume = new ::PolyVox::RawVolume<MultiMaterial>(region);
		}
		else
		{
			// If if does exist then make sure it's the right size.
			if(mSmoothingVolume->getEnclosingRegion() != region)
			{
				delete mSmoothingVolume;
				mSmoothingVolume = new ::PolyVox::RawVolume<MultiMaterial>(region);
			}
		}

		for(int z = firstZ; z <= lastZ; ++z)
		{
			for(int y = firstY; y <= lastY; ++y)
			{
				for(int x = firstX; x <= lastX; ++x)
				{					
					for(uint32_t matIndex = 0; matIndex < MultiMaterial::getNoOfMaterials(); matIndex++)
					{
						uint32_t sum = 0;

						sum += mSmoothTerrainVolume->getVoxelAt(x, y, z).getMaterial(matIndex);
						sum += mSmoothTerrainVolume->getVoxelAt(x+1, y, z).getMaterial(matIndex);
						sum += mSmoothTerrainVolume->getVoxelAt(x-1, y, z).getMaterial(matIndex);
						sum += mSmoothTerrainVolume->getVoxelAt(x, y+1, z).getMaterial(matIndex);
						sum += mSmoothTerrainVolume->getVoxelAt(x, y-1, z).getMaterial(matIndex);
						sum += mSmoothTerrainVolume->getVoxelAt(x, y, z+1).getMaterial(matIndex);
						sum += mSmoothTerrainVolume->getVoxelAt(x, y, z-1).getMaterial(matIndex);

						uint32_t average = sum / 7;
						uint32_t rem = sum % 7;
						//if(rem > 3)
						{
							average++;
							average++;
							average++;
							average++;
						}

						MultiMaterial result = mSmoothingVolume->getVoxelAt(x, y, z);
						result.setMaterial(matIndex, average);
						mSmoothingVolume->setVoxel(x, y, z, result);
					}

					MultiMaterial result = mSmoothingVolume->getVoxelAt(x, y, z);
					result.clampSumOfMaterials();
					mSmoothingVolume->setVoxel(x, y, z, result);
				}
			}
		}

		for(int z = firstZ; z <= lastZ; ++z)
		{
			for(int y = firstY; y <= lastY; ++y)
			{
				for(int x = firstX; x <= lastX; ++x)
				{
					mSmoothTerrainVolume->setVoxelAt(x, y, z, mSmoothingVolume->getVoxelAt(x, y, z), UpdatePriorities::DontUpdate);
				}
			}
		}

		mSmoothTerrainVolume->markAsModified(region, UpdatePriorities::Immediate);
	}

	void SmoothTerrainVolumeEditor::subtractFromMaterial(uint8_t amountToAdd, MultiMaterial& material)
	{
		uint32_t indexToRemoveFrom = 0; //FIXME - start somewhere random
		uint32_t iterationWithNoRemovals = 0;
		while(amountToAdd > 0)
		{
			//if(indexToRemoveFrom != index)
			{
				if(material.getMaterial(indexToRemoveFrom) > 0)
				{
					//material.setMaterial(index, material.getMaterial(index) + 1);
					material.setMaterial(indexToRemoveFrom, material.getMaterial(indexToRemoveFrom) - 1);
					amountToAdd--;
					iterationWithNoRemovals = 0;
				}
				else
				{
					iterationWithNoRemovals++;
				}
			}
			/*else
			{
				iterationWithNoRemovals++;
			}*/

			if(iterationWithNoRemovals == MultiMaterial::getNoOfMaterials())
			{
				break;
			}

			indexToRemoveFrom++;
			indexToRemoveFrom %= MultiMaterial::getNoOfMaterials();
		}
	}

	void SmoothTerrainVolumeEditor::addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial& material)
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

	void sculptSmoothTerrainVolume(SmoothTerrainVolumeImpl* smoothTerrainVolume, const Vector3F& centre, float brushRadius, float amount)
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

						

						//int32_t iAverage = (std::max)(original, sample);

						int32_t iAverage = sample;

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

						//fAverage *= (1.0f + gaussian);
						//fAverage += gaussian;

						float fLerped = lerp(static_cast<float>(original), fAverage, gaussian);

						int32_t iLerped = static_cast<int32_t>(fLerped + 0.5f);

						

						/*if(speed >= 0.0f)
						{
							iAverage = (std::max)(iAverage, original); // For some reason material gets slightly eroded unless we use this.
						}
						if(speed <= 0.0f)
						{
							iAverage = (std::min)(iAverage, original); // For some reason material gets slightly dilated unless we use this.
						}*/

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

		/*for(int z = firstZ; z <= lastZ; ++z)
		{
			for(int y = firstY; y <= lastY; ++y)
			{
				for(int x = firstX; x <= lastX; ++x)
				{
					smoothTerrainVolume->setVoxelAt(x, y, z, mSmoothingVolume.getVoxelAt(x, y, z), UpdatePriorities::DontUpdate);
				}
			}
		}*/

		smoothTerrainVolume->markAsModified(region, UpdatePriorities::Immediate);
	}
}