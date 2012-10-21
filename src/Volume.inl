#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Raycast.h"

#include "GameplayMarchingCubesController.h"
#include "GameplayIsQuadNeeded.h"

template <typename VoxelType>
class RaycastTestFunctor
{
public:
	RaycastTestFunctor()
	{
	}

	bool operator()(Vector3DFloat pos, const VoxelType& voxel)
	{
	}
};

template <>
class RaycastTestFunctor<MultiMaterial>
{
public:
	RaycastTestFunctor()
	{
	}

	bool operator()(Vector3DFloat pos, const MultiMaterial& voxel)
	{
		mLastPos = pos;
		return voxel.getSumOfMaterials() <= MultiMaterial::getMaxMaterialValue() / 2;
	}

	Vector3DFloat mLastPos;
};

template <>
class RaycastTestFunctor<Colour>
{
public:
	RaycastTestFunctor()
	{
	}

	bool operator()(Vector3DFloat pos, const Colour& voxel)
	{
		return false;
	}

	Vector3DFloat mLastPos;
};

template <typename VoxelType>
Volume<VoxelType>::Volume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	:mVolData(0)
	,mRootNode(0)
	//,mVolumeRegion(0)
	,mType(type)
	,mRegionWidth(regionWidth)
	,mRegionHeight(regionHeight)
	,mRegionDepth(regionDepth)
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
	mVolData = new SimpleVolume<VoxelType>(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ));

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

				// The above actually causes the the regions to extend outside the upper range of
				// the volume. For the Marching cubes this is fine as it ensures the volume will
				// get closed, so we wnt to mimic this behaviour on the lower edges too.
				if(getType() == VolumeTypes::SmoothTerrain)
				{
					if(x == 0) regLowerX--;
					if(y == 0) regLowerY--;
					if(z == 0) regLowerZ--;
				}

				// This wasn't necessary for the coloured cubes because this surface extractor already
				// peeks outside the region in the negative direction. But we do need to add a gap between
				// the regions for the cubic surface extractor as in this case voxels should not be shared
				// between regions (see the cubic surface extractor docs for a diagram). However, we skip
				// this for the upper extremes as we do want to preserve the property of the regions
				// extending outside the volumes (to close off the mesh).
				if(getType() == VolumeTypes::ColouredCubes)
				{
					if(x < (volumeWidthInRegions - 1)) regUpperX--;
					if(y < (volumeHeightInRegions - 1)) regUpperY--;
					if(z < (volumeDepthInRegions - 1)) regUpperZ--;
				}

				mVolumeRegions[x][y][z] = new VolumeRegion(Region(regLowerX, regLowerY, regLowerZ, regUpperX, regUpperY, regUpperZ));
				mRootNode->addChild(mVolumeRegions[x][y][z]->mNode);
				//mVolumeRegions[x][y][z]->mNode->setTranslation(regLowerX, regLowerY, regLowerZ);
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
VoxelType Volume<VoxelType>::getVoxelAt(int x, int y, int z)
{
	return mVolData->getVoxelAt(x, y, z);
}

template <typename VoxelType>
void Volume<VoxelType>::setVoxelAt(int x, int y, int z, VoxelType value)
{
	mVolData->setVoxelAt(x, y, z, value);

	int regionX = x / mRegionWidth;
	int regionY = y / mRegionHeight;
	int regionZ = z / mRegionDepth;
	mVolumeRegions[regionX][regionY][regionZ]->mIsMeshUpToDate = false;
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

	VoxelType value;

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < mVolData->getWidth(); z++)
	{
		for (int y = 0; y < mVolData->getHeight(); y++)
		{
			for (int x = 0; x < mVolData->getDepth(); x++)
			{
				//Slow and inefficient reading one voxel at a time!
				size_t elementsRead = fread(&value, sizeof(VoxelType), 1,inputFile);

				if(elementsRead != 1)
				{
					GP_ERROR("Failed to read voxel %d, %d, %d", x, y, z);
				}

				//Write the voxel value into the volume
				setVoxelAt(x, y, z, value);

			}
		}
	}

	fclose(inputFile);
}

template <typename VoxelType>
void Volume<VoxelType>::saveData(const char* filename)
{
	FILE* outputFile = fopen(filename, "wb");
	if(!outputFile)
	{
		GP_ERROR("Failed to open volume file");
	}

	uint16_t width = mVolData->getWidth();
	uint16_t height = mVolData->getHeight();
	uint16_t depth = mVolData->getDepth();
	fwrite(&width, sizeof(uint16_t), 1, outputFile);
	fwrite(&height, sizeof(uint16_t), 1, outputFile);
	fwrite(&depth, sizeof(uint16_t), 1, outputFile);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < mVolData->getWidth(); z++)
	{
		for (int y = 0; y < mVolData->getHeight(); y++)
		{
			for (int x = 0; x < mVolData->getDepth(); x++)
			{
				VoxelType value = mVolData->getVoxelAt(x,y,z);
				fwrite(&value, sizeof(VoxelType), 1,outputFile);
			}
		}
	}

	fclose(outputFile);
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
				if(mVolumeRegions[x][y][z]->mIsMeshUpToDate == false)
				{
					Region regionToExtract = mVolumeRegions[x][y][z]->mRegion;
					//Extract the surface
					if(getType() == VolumeTypes::ColouredCubes)
					{
						GameplayIsQuadNeeded<VoxelType> isQuadNeeded;
						SurfaceMesh<PositionMaterial<VoxelType> > colouredCubicMesh;
						CubicSurfaceExtractor< SimpleVolume<VoxelType>, GameplayIsQuadNeeded<VoxelType> > surfaceExtractor(mVolData, regionToExtract, &colouredCubicMesh, true, isQuadNeeded);
						surfaceExtractor.execute();

						if(colouredCubicMesh.getNoOfIndices() > 0)
						{
							mVolumeRegions[x][y][z]->buildGraphicsMesh(colouredCubicMesh);
						}
					}
					else if(getType() == VolumeTypes::SmoothTerrain)
					{
						GameplayMarchingCubesController<VoxelType> controller;
						SurfaceMesh<PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > > smoothTerrainMesh;
						MarchingCubesSurfaceExtractor< SimpleVolume<VoxelType>, GameplayMarchingCubesController<VoxelType> > surfaceExtractor(mVolData, regionToExtract, &smoothTerrainMesh, controller);
						surfaceExtractor.execute();

						if(smoothTerrainMesh.getNoOfIndices() > 0)
						{
							mVolumeRegions[x][y][z]->buildGraphicsMesh(smoothTerrainMesh);
						}
					}

					// FIXME - We shouldn't really set this here as it's not changing every time we update the mesh data.
					// But before deciding on a material handling strategy let's see what options we come up with for texturing smooth terrain.
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

					mVolumeRegions[x][y][z]->mIsMeshUpToDate = true;
				}
			}
		}
	}
}

template <typename VoxelType>
bool Volume<VoxelType>::raycast(Ray startAndDirection, float length, Vector3& result)
{
	if(getType() == VolumeTypes::SmoothTerrain)
	{
		Vector3DFloat v3dStart(startAndDirection.getOrigin().x, startAndDirection.getOrigin().y, startAndDirection.getOrigin().z);
		Vector3DFloat v3dDirection(startAndDirection.getDirection().x, startAndDirection.getDirection().y, startAndDirection.getDirection().z);
		v3dDirection *= length;

		RaycastTestFunctor<VoxelType> raycastTestFunctor;
		RaycastResult myResult = smoothRaycastWithDirection(mVolData, v3dStart, v3dDirection, raycastTestFunctor, 0.5f);
		if(myResult == RaycastResults::Interupted)
		{
			result = Vector3(raycastTestFunctor.mLastPos.getX(), raycastTestFunctor.mLastPos.getY(), raycastTestFunctor.mLastPos.getZ());
			return true;
		}
	}

	return false;
}
