#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

class GameplayMarchingCubesController
{
public:
	typedef float DensityType;
	typedef float MaterialType;

	float convertToDensity(Material16 voxel)
	{
		if(voxel.getMaterial() > 0)
		{
			return 100.0f;
		}
		else
		{
			return 0.0f;
		}
	}

	float convertToMaterial(Material16 voxel)
	{
		return 1;
	}

	float getThreshold(void)
	{
		return 50.0f;
	}
};

template <typename VoxelType>
Volume<VoxelType>::Volume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	:mVolData(0)
	,mRootNode(0)
	//,mVolumeRegion(0)
	,mType(type)
{
	int volumeWidth = (upperX - lowerX) + 1;
	int volumeHeight = (upperY - lowerY) + 1;
	int volumeDepth = (upperZ - lowerZ) + 1;
	GP_ASSERT(volumeWidth > 0);
	GP_ASSERT(volumeHeight > 0);
	GP_ASSERT(volumeDepth > 0);
	GP_ASSERT(volumeWidth % regionWidth == 0);
	GP_ASSERT(volumeHeight % regionHeight == 0);
	GP_ASSERT(volumeDepth % regionDepth == 0);

	mVolData = new SimpleVolume<Material16>(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ));
	mRootNode = Node::create();

	unsigned int volumeWidthInRegions = volumeWidth / regionWidth;
	unsigned int volumeHeightInRegions = volumeHeight / regionHeight;
	unsigned int volumeDepthInRegions = volumeDepth / regionDepth;
	mVolumeRegions.resize(ArraySizes(volumeWidthInRegions)(volumeHeightInRegions)(volumeDepthInRegions));
	for(int z = 0; z < volumeDepthInRegions; z++)
	{
		for(int y = 0; y < volumeHeightInRegions; y++)
		{
			for(int x = 0; x < volumeWidthInRegions; x++)
			{
				// Set up the regions so they exactly touch and neighbouring regions share
				// voxels on thier faces. This is what we need for the Marching Cubes surface
				int regLowerX = lowerX + x * regionWidth;
				int regLowerY = lowerY + y * regionHeight;
				int regLowerZ = lowerZ + z * regionDepth;
				int regUpperX = regLowerX + regionWidth;
				int regUpperY = regLowerY + regionHeight;
				int regUpperZ = regLowerZ + regionDepth;

				mVolumeRegions[x][y][z] = new VolumeRegion(Region(regLowerX, regLowerY, regLowerZ, regUpperX, regUpperY, regUpperZ));
				mRootNode->addChild(mVolumeRegions[x][y][z]->mNode);
				mVolumeRegions[x][y][z]->mNode->translate(regLowerX, regLowerY, regLowerZ);
			}
		}
	}
}

template <typename VoxelType>
Volume<VoxelType>::~Volume()
{
	SAFE_RELEASE(mRootNode);
}

template <typename VoxelType>
Node* Volume<VoxelType>::getRootNode()
{
	return mRootNode;
}

template <typename VoxelType>
VolumeType Volume<VoxelType>::getType(void) const
{
	return mType;
}

template <typename VoxelType>
void Volume<VoxelType>::setVoxelAt(int x, int y, int z, VoxelType value)
{
	mVolData->setVoxelAt(x, y, z, value);
}

template <typename VoxelType>
void Volume<VoxelType>::loadData(const char* filename)
{
	FILE* inputFile = fopen(filename, "rb");
	if(!inputFile)
	{
		GP_ERROR("Failed to open volume file");
	}

	fseek(inputFile, 6, SEEK_SET);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < mVolData->getWidth(); z++)
	{
		for (int y = 0; y < mVolData->getHeight(); y++)
		{
			for (int x = 0; x < mVolData->getDepth(); x++)
			{
				uint16_t diskVal;

				//Slow and inefficient reading one voxel at a time!
				size_t elementsRead = fread(&diskVal, sizeof(diskVal), 1,inputFile);
				if(elementsRead != 1)
				{
					GP_ERROR("Failed to read voxel %d, %d, %d", x, y, z);
				}

				//HACK - For some reason the valures coming from Voxeliens
				//seem to have the endianness the wrong way round? Swap them.
				diskVal = (((diskVal & 0xff)<<8) | ((diskVal & 0xff00)>>8));

				//Wrte the voxel value into the volume	
				Material16 voxel(diskVal);
				setVoxelAt(x, y, z, voxel);
			}
		}
	}

	fclose(inputFile);
}

template <typename VoxelType>
void Volume<VoxelType>::updateMeshes()
{
	for(int z = 0; z < mVolumeRegions.getDimension(2); z++)
	{
		for(int y = 0; y < mVolumeRegions.getDimension(1); y++)
		{
			for(int x = 0; x < mVolumeRegions.getDimension(0); x++)
			{
				Region regionToExtract = mVolumeRegions[x][y][z]->mRegion;
				//Extract the surface
				if(getType() == VolumeTypes::ColouredCubes)
				{
					SurfaceMesh<PositionMaterial> colouredCubicMesh;
					CubicSurfaceExtractor< SimpleVolume<Material16> > surfaceExtractor(mVolData, regionToExtract, &colouredCubicMesh);
					surfaceExtractor.execute();

					if(colouredCubicMesh.getNoOfIndices() > 0)
					{
						mVolumeRegions[x][y][z]->buildGraphicsMesh(colouredCubicMesh);
					}
				}
				else if(getType() == VolumeTypes::SmoothTerrain)
				{
					SurfaceMesh<PositionMaterialNormal> smoothTerrainMesh;
					GameplayMarchingCubesController controller;					
					MarchingCubesSurfaceExtractor< SimpleVolume<Material16>, GameplayMarchingCubesController > surfaceExtractor(mVolData, regionToExtract, &smoothTerrainMesh, controller);
					surfaceExtractor.execute();

					if(smoothTerrainMesh.getNoOfIndices() > 0)
					{
						mVolumeRegions[x][y][z]->buildGraphicsMesh(smoothTerrainMesh);
					}
				}

				switch(getType())
				{
				case VolumeTypes::ColouredCubes:
					mVolumeRegions[x][y][z]->setMaterial("res/PolyVox.material");
					break;
				case VolumeTypes::SmoothTerrain:
					mVolumeRegions[x][y][z]->setMaterial("res/SmoothTerrain.material");
					break;
				default:
					//Add fallback material here
					break;
				}	
			}
		}
	}
}
