#ifndef CUBIQUITY_COLOUREDCUBICSURFACEEXTRACTIONTASK_H_
#define CUBIQUITY_COLOUREDCUBICSURFACEEXTRACTIONTASK_H_

#include "Colour.h"
#include "CubiquityForwardDeclarations.h"
#include "Task.h"
#include "VoxelTraits.h"

class ColouredCubicSurfaceExtractionTask : public Task
{
public:
	ColouredCubicSurfaceExtractionTask(OctreeNode< Colour >* octreeNode, PolyVox::SimpleVolume<Colour>* polyVoxVolume);
	~ColouredCubicSurfaceExtractionTask();

	void process(void);

public:
	OctreeNode< Colour >* mOctreeNode;
	PolyVox::SimpleVolume<Colour>* mPolyVoxVolume;
	PolyVox::SurfaceMesh<PolyVox::PositionMaterial<Colour> >* mColouredCubicMesh;
};

template< typename SrcVolumeType, typename DstVolumeType>
void rescaleCubicVolume(SrcVolumeType* pVolSrc, const PolyVox::Region& regSrc, DstVolumeType* pVolDst, const PolyVox::Region& regDst)
{
	POLYVOX_ASSERT(regSrc.getWidthInVoxels() == regDst.getWidthInVoxels() * 2, "Wrong size!");
	POLYVOX_ASSERT(regSrc.getHeightInVoxels() == regDst.getHeightInVoxels() * 2, "Wrong size!");
	POLYVOX_ASSERT(regSrc.getDepthInVoxels() == regDst.getDepthInVoxels() * 2, "Wrong size!");

	SrcVolumeType::Sampler srcSampler(pVolSrc);
	DstVolumeType::Sampler dstSampler(pVolDst);

	// First of all we iterate over all destination voxels and compute their colour as the
	// average of the colours of the eight corresponding voxels in the higher resolution version.
	for(int32_t z = 0; z < regDst.getDepthInVoxels(); z++)
	{
		for(int32_t y = 0; y < regDst.getHeightInVoxels(); y++)
		{
			for(int32_t x = 0; x < regDst.getWidthInVoxels(); x++)
			{
				Vector3DInt32 srcPos = regSrc.getLowerCorner() + (Vector3DInt32(x, y, z) * 2);
				Vector3DInt32 dstPos = regDst.getLowerCorner() + Vector3DInt32(x, y, z);

				uint32_t noOfSolidVoxels = 0;
				uint32_t averageOf8Red = 0;
				uint32_t averageOf8Green = 0;
				uint32_t averageOf8Blue = 0;
				for(int32_t childZ = 0; childZ < 2; childZ++)
				{
					for(int32_t childY = 0; childY < 2; childY++)
					{
						for(int32_t childX = 0; childX < 2; childX++)
						{
							srcSampler.setPosition(srcPos + Vector3DInt32(childX, childY, childZ));

							Colour child = srcSampler.getVoxel();

							if(child.getAlpha () > 0)
							{
								noOfSolidVoxels++;
								averageOf8Red += child.getRed();
								averageOf8Green += child.getGreen();
								averageOf8Blue += child.getBlue();
							}
						}
					}
				}

				// We only make a voxel solid if the eight corresponding voxels are also all solid. This
				// means that higher LOD meshes actually shrink away which ensures cracks aren't visible.
				if(noOfSolidVoxels > 7)
				{
					Colour colour;
					colour.setColour(averageOf8Red / noOfSolidVoxels, averageOf8Green / noOfSolidVoxels, averageOf8Blue / noOfSolidVoxels, 15);
					pVolDst->setVoxelAt(dstPos, colour);
				}
				else
				{
					Colour colour;
					colour.setColour(0,0,0,0);
					pVolDst->setVoxelAt(dstPos, colour);
				}
			}
		}
	}

	// At this point the results are usable, but we have a problem with thin structures disappearing.
	// For example, if we have a solid blue sphere with a one voxel thick layer of red voxels on it,
	// then we don't care that the shape changes then the red voxels are lost but we do care that the
	// colour changes, as this is very noticable. Our solution is o process again only those voxels
	// which lie on a material-air boundary, and to recompute their colour using a larger naighbourhood
	// while also accounting for how visible the child voxels are.
	for(int32_t z = 0; z < regDst.getDepthInVoxels(); z++)
	{
		for(int32_t y = 0; y < regDst.getHeightInVoxels(); y++)
		{
			for(int32_t x = 0; x < regDst.getWidthInVoxels(); x++)
			{
				Vector3DInt32 dstPos = regDst.getLowerCorner() + Vector3DInt32(x, y, z);

				dstSampler.setPosition(dstPos);

				//Skip empty voxels
				if(dstSampler.getVoxel().getAlpha() > 0)
				{
					//Only process voxels on a material-air boundary.
					if((dstSampler.peekVoxel0px0py1nz().getAlpha() == 0) ||
					   (dstSampler.peekVoxel0px0py1pz().getAlpha() == 0) || 
					   (dstSampler.peekVoxel0px1ny0pz().getAlpha() == 0) ||
					   (dstSampler.peekVoxel0px1py0pz().getAlpha() == 0) ||
					   (dstSampler.peekVoxel1nx0py0pz().getAlpha() == 0) || 
					   (dstSampler.peekVoxel1px0py0pz().getAlpha() == 0))
					{
						Vector3DInt32 srcPos = regSrc.getLowerCorner() + (Vector3DInt32(x, y, z) * 2);

						uint32_t totalRed = 0;
						uint32_t totalGreen = 0;
						uint32_t totalBlue = 0;
						uint32_t totalExposedFaces = 0;

						// Look ate the 64 (4x4x4) children
						for(int32_t childZ = -1; childZ < 3; childZ++)
						{
							for(int32_t childY = -1; childY < 3; childY++)
							{
								for(int32_t childX = -1; childX < 3; childX++)
								{
									srcSampler.setPosition(srcPos + Vector3DInt32(childX, childY, childZ));

									Colour child = srcSampler.getVoxel();

									if(child.getAlpha () > 0)
									{
										// For each small voxel, count the exposed faces and use this
										// to determine the importance of the colour contribution.
										uint32_t exposedFaces = 0;
										if(srcSampler.peekVoxel0px0py1nz().getAlpha() == 0) exposedFaces++;
										if(srcSampler.peekVoxel0px0py1pz().getAlpha() == 0) exposedFaces++;
										if(srcSampler.peekVoxel0px1ny0pz().getAlpha() == 0) exposedFaces++;
										if(srcSampler.peekVoxel0px1py0pz().getAlpha() == 0) exposedFaces++;
										if(srcSampler.peekVoxel1nx0py0pz().getAlpha() == 0) exposedFaces++;
										if(srcSampler.peekVoxel1px0py0pz().getAlpha() == 0) exposedFaces++;

										totalRed += child.getRed() * exposedFaces;
										totalGreen += child.getGreen() * exposedFaces;
										totalBlue += child.getBlue() * exposedFaces;

										totalExposedFaces += exposedFaces;
									}							
								}
							}
						}

						// Avoid divide by zero if there were no exposed faces.
						if(totalExposedFaces == 0) totalExposedFaces++;

						Colour colour;
						colour.setColour(totalRed / totalExposedFaces, totalGreen / totalExposedFaces, totalBlue / totalExposedFaces, 15);
						pVolDst->setVoxelAt(dstPos, colour);
					}
				}
			}
		}
	}
}

#endif //CUBIQUITY_COLOUREDCUBICSURFACEEXTRACTIONTASK_H_
