#include "Volume.h"

#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

Volume::Volume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	:mVolData(0)
	,mRootNode(0)
	//,mVolumeRegion(0)
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

	mRootNode = Node::create();
	//mVolumeRegion = new VolumeRegion(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ));
	//mRootNode->addChild(mVolumeRegion->mNode);
	mVolData = new SimpleVolume<Material16>(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ));

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
				int regLowerX = lowerX + x * regionWidth;
				int regLowerY = lowerY + y * regionHeight;
				int regLowerZ = lowerZ + z * regionDepth;
				int regUpperX = regLowerX + regionWidth - 1;
				int regUpperY = regLowerY + regionHeight - 1;
				int regUpperZ = regLowerZ + regionDepth - 1;
				mVolumeRegions[x][y][z] = new VolumeRegion(Region(regLowerX, regLowerY, regLowerZ, regUpperX, regUpperY, regUpperZ));
				mRootNode->addChild(mVolumeRegions[x][y][z]->mNode);
				//mVolumeRegions[x][y][z]->mNode->setTranslation(regLowerX, regLowerY, regLowerZ);
				mVolumeRegions[x][y][z]->mNode->translate(regLowerX, regLowerY, regLowerZ);
			}
		}
	}
}

Volume::~Volume()
{
	SAFE_RELEASE(mRootNode);
}

Volume* Volume::create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
{
	Volume* volume = new Volume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);
	return volume;
}

Node* Volume::getRootNode()
{
	return mRootNode;
}

void Volume::setMaterial(const char* materialPath)
{
	for(int z = 0; z < mVolumeRegions.getDimension(2); z++)
	{
		for(int y = 0; y < mVolumeRegions.getDimension(1); y++)
		{
			for(int x = 0; x < mVolumeRegions.getDimension(0); x++)
			{
				mVolumeRegions[x][y][z]->mNode->getModel()->setMaterial(materialPath);
			}
		}
	}
}

void Volume::setVoxelAt(int x, int y, int z, PolyVox::Material16 value)
{
	mVolData->setVoxelAt(x, y, z, value);
}

void Volume::loadData(const char* filename)
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

void Volume::updateMeshes()
{
	for(int z = 0; z < mVolumeRegions.getDimension(2); z++)
	{
		for(int y = 0; y < mVolumeRegions.getDimension(1); y++)
		{
			for(int x = 0; x < mVolumeRegions.getDimension(0); x++)
			{
				//Extract the surface
				SurfaceMesh<PositionMaterial> polyVoxMesh;
				CubicSurfaceExtractor< SimpleVolume<Material16> > surfaceExtractor(mVolData, mVolumeRegions[x][y][z]->mRegion, &polyVoxMesh);
				surfaceExtractor.execute();	

				if(polyVoxMesh.getNoOfIndices() > 0)
				{
					mVolumeRegions[x][y][z]->buildGraphicsMesh(polyVoxMesh);
				}
			}
		}
	}
}
