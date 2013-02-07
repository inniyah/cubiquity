#include "SmoothTerrainVolumeEditor.h"

using namespace PolyVox;

SmoothTerrainVolumeEditor::SmoothTerrainVolumeEditor(SmoothTerrainVolume* volume)
	:mVolume(volume)
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

void SmoothTerrainVolumeEditor::edit(const PolyVox::Vector3DFloat& centre, float radius, uint32_t materialToUse, EditAction editAction, float timeElapsedInSeconds, float amount, float smoothBias)
{
	int firstX = static_cast<int>(std::floor(centre.getX() - radius));
	int firstY = static_cast<int>(std::floor(centre.getY() - radius));
	int firstZ = static_cast<int>(std::floor(centre.getZ() - radius));

	int lastX = static_cast<int>(std::ceil(centre.getX() + radius));
	int lastY = static_cast<int>(std::ceil(centre.getY() + radius));
	int lastZ = static_cast<int>(std::ceil(centre.getZ() + radius));

	float radiusSquared = radius * radius;

	//Check bounds.
	firstX = std::max(firstX,mVolume->mVolData->getEnclosingRegion().getLowerCorner().getX());
	firstY = std::max(firstY,mVolume->mVolData->getEnclosingRegion().getLowerCorner().getY());
	firstZ = std::max(firstZ,mVolume->mVolData->getEnclosingRegion().getLowerCorner().getZ());

	lastX = std::min(lastX,mVolume->mVolData->getEnclosingRegion().getUpperCorner().getX());
	lastY = std::min(lastY,mVolume->mVolData->getEnclosingRegion().getUpperCorner().getY());
	lastZ = std::min(lastZ,mVolume->mVolData->getEnclosingRegion().getUpperCorner().getZ());

	PolyVox::Region region(firstX, firstY, firstZ, lastX, lastY, lastZ);

	// Do the smoothing once, before we start iterating over all the voxels.
	// At a per-voxel level we only need to do the interpolation.
	if(editAction == EditActions::Smooth)
	{
		// Make sure our temporary target exists, and create it if not.
		if(!mSmoothingVolume)
		{
			mSmoothingVolume = new PolyVox::RawVolume<MultiMaterial4>(region);
		}
		else
		{
			// If if does exist then make sure it's the right size.
			if(mSmoothingVolume->getEnclosingRegion() != region)
			{
				delete mSmoothingVolume;
				mSmoothingVolume = new PolyVox::RawVolume<MultiMaterial4>(region);
			}
		}

		// We might not need to do this at float precision, it should be tested again.
		PolyVox::LowPassFilter< PolyVox::SimpleVolume<MultiMaterial4>, PolyVox::RawVolume<MultiMaterial4>, PolyVox::Vector<4, float> > lowPassFilter(mVolume->mVolData, region, mSmoothingVolume, region, 3);
		lowPassFilter.execute();
	}

	for(int z = firstZ; z <= lastZ; ++z)
	{
		for(int y = firstY; y <= lastY; ++y)
		{
			for(int x = firstX; x <= lastX; ++x)
			{
				if((centre - Vector3DFloat(x,y,z)).lengthSquared() <= radiusSquared)
				{
					float falloff = falloff = (centre - Vector3DFloat(x,y,z)).length() / radius;
					falloff = (std::min)((std::max)(falloff, 0.0f), 1.0f);
					falloff = 1.0f - falloff;

					// Falloff will be one for most of the sphere and will
					// then rapidly drop to zero over the last few voxels.
					falloff *= 0.3f;
					falloff += 0.7f;

					float amountToEditBy = falloff * timeElapsedInSeconds * amount * static_cast<float>(MultiMaterial4::getMaxMaterialValue());

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
							MultiMaterial4 originalMat = mVolume->getVoxelAt(x, y, z);	
							uint32_t sumOfMaterials = originalMat.getSumOfMaterials();
							if(sumOfMaterials + uToAddOrSubtract <= originalMat.getMaxMaterialValue())
							{
								originalMat.setMaterial(materialToUse, originalMat.getMaterial(materialToUse) + uToAddOrSubtract);
							}
							else
							{
								addToMaterial(materialToUse, uToAddOrSubtract, originalMat);
							}
							mVolume->setVoxelAt(x,y,z, originalMat);
							break;
						}
					case EditActions::Subtract:
						{
							MultiMaterial4 originalMat = mVolume->getVoxelAt(x, y, z);	
							uint32_t sumOfMaterials = originalMat.getSumOfMaterials();
							subtractFromMaterial(uToAddOrSubtract, originalMat);
							mVolume->setVoxelAt(x,y,z, originalMat);
							break;
						}
					case EditActions::Paint:
						{						
							MultiMaterial4 originalMat = mVolume->getVoxelAt(x, y, z);	
							addToMaterial(materialToUse, uToAddOrSubtract, originalMat);
							mVolume->setVoxelAt(x,y,z, originalMat);
							break;
						}
					case EditActions::Smooth:
						{		
							MultiMaterial4 originalMat = mVolume->getVoxelAt(x, y, z);
							MultiMaterial4 smoothedMat = mSmoothingVolume->getVoxelAt(x, y, z);

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

							MultiMaterial4 interpMat;
							// In theory we should add 0.5f before casting to round
							// properly, but this seems to cause material to grow too much.
							// Instead we add a user-supplied bias value.
							float bias = smoothBias;
							interpMat.setMaterial(0, (std::max<uint32_t>)(0, (std::min)(originalMat.getMaxMaterialValue(), static_cast<uint32_t>(interp0 + bias))));
							interpMat.setMaterial(1, (std::max<uint32_t>)(0, (std::min)(originalMat.getMaxMaterialValue(), static_cast<uint32_t>(interp1 + bias))));
							interpMat.setMaterial(2, (std::max<uint32_t>)(0, (std::min)(originalMat.getMaxMaterialValue(), static_cast<uint32_t>(interp2 + bias))));
							interpMat.setMaterial(3, (std::max<uint32_t>)(0, (std::min)(originalMat.getMaxMaterialValue(), static_cast<uint32_t>(interp3 + bias))));

							mVolume->setVoxelAt(x,y,z, interpMat);

							break;
						}
					}
				}
			}
		}
	}
}

void SmoothTerrainVolumeEditor::subtractFromMaterial(uint8_t amountToAdd, MultiMaterial4& material)
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

		if(iterationWithNoRemovals == MultiMaterial4::getNoOfMaterials())
		{
			break;
		}

		indexToRemoveFrom++;
		indexToRemoveFrom %= MultiMaterial4::getNoOfMaterials();
	}
}

void SmoothTerrainVolumeEditor::addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial4& material)
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

		if(iterationWithNoRemovals == MultiMaterial4::getNoOfMaterials())
		{
			break;
		}

		indexToRemoveFrom++;
		indexToRemoveFrom %= MultiMaterial4::getNoOfMaterials();
	}
}