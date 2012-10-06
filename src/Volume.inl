#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

#include "PolyVoxCore/MaterialDensityPair.h"


//We never actually call the marching cubes algorithm on a Material16 volume but we need to be able to compile it.
template<>
class DefaultMarchingCubesController< Material16 >
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
void Volume<VoxelType>::setVoxelAt(int x, int y, int z, VoxelType value)
{
	mVolData->setVoxelAt(x, y, z, value);

	int regionX = x / mRegionWidth;
	int regionY = y / mRegionHeight;
	int regionZ = z / mRegionDepth;
	mVolumeRegions[regionX][regionY][regionZ]->mIsMeshUpToDate = false;
}

template <typename VoxelType>
void Volume<VoxelType>::createSphereAt(const gameplay::Vector3& centre, float radius, VoxelType value)
{
	int firstX = static_cast<int>(std::floor(centre.x - radius));
	int firstY = static_cast<int>(std::floor(centre.y - radius));
	int firstZ = static_cast<int>(std::floor(centre.z - radius));

	int lastX = static_cast<int>(std::ceil(centre.x + radius));
	int lastY = static_cast<int>(std::ceil(centre.y + radius));
	int lastZ = static_cast<int>(std::ceil(centre.z + radius));

	float radiusSquared = radius * radius;

	//Check bounds.
	firstX = std::max(firstX,mVolData->getEnclosingRegion().getLowerCorner().getX());
	firstY = std::max(firstY,mVolData->getEnclosingRegion().getLowerCorner().getY());
	firstZ = std::max(firstZ,mVolData->getEnclosingRegion().getLowerCorner().getZ());

	lastX = std::min(lastX,mVolData->getEnclosingRegion().getUpperCorner().getX());
	lastY = std::min(lastY,mVolData->getEnclosingRegion().getUpperCorner().getY());
	lastZ = std::min(lastZ,mVolData->getEnclosingRegion().getUpperCorner().getZ());

	for(int z = firstZ; z <= lastZ; ++z)
	{
		for(int y = firstY; y <= lastY; ++y)
		{
			for(int x = firstX; x <= lastX; ++x)
			{
				if((centre - Vector3(x,y,z)).lengthSquared() <= radiusSquared)
				{
					setVoxelAt(x,y,z,value);
				}
			}
		}
	}
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
				size_t elementsRead = fread(&diskVal, sizeof(uint16_t), 1,inputFile);
				if(elementsRead != 1)
				{
					GP_ERROR("Failed to read voxel %d, %d, %d", x, y, z);
				}

				//HACK - For some reason the valures coming from Voxeliens
				//seem to have the endianness the wrong way round? Swap them.
				diskVal = (((diskVal & 0xff)<<8) | ((diskVal & 0xff00)>>8));

				//Wrte the voxel value into the volume

				//VolumeTypes::ColouredCubes
				/*VoxelType voxel(diskVal);
				setVoxelAt(x, y, z, voxel);*/

				//VolumeTypes::SmoothTerrain
				VoxelType voxel;				
				Vector3DFloat material(0,0,0);

				if(diskVal == 34383) // Soil
				{				
					material.setX(1);					
				}
				else if(diskVal == 18751) // Grass
				{				
					material.setY(1);					
				}
				else if(diskVal == 255) // Water
				{				
					material.setZ(1);					
				}
				else
				{
					//voxel.setMaterial(0);
					//GP_WARN("%d", diskVal);
				}

				voxel.setMaterial(material);

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
				if(mVolumeRegions[x][y][z]->mIsMeshUpToDate == false)
				{
					Region regionToExtract = mVolumeRegions[x][y][z]->mRegion;
					//Extract the surface
					if(getType() == VolumeTypes::ColouredCubes)
					{
						/*SurfaceMesh<PositionMaterial> colouredCubicMesh;
						CubicSurfaceExtractor< SimpleVolume<VoxelType> > surfaceExtractor(mVolData, regionToExtract, &colouredCubicMesh);
						surfaceExtractor.execute();

						if(colouredCubicMesh.getNoOfIndices() > 0)
						{
							mVolumeRegions[x][y][z]->buildGraphicsMesh(colouredCubicMesh);
						}*/
					}
					else if(getType() == VolumeTypes::SmoothTerrain)
					{
						SurfaceMesh<PositionMaterialNormal< DefaultMarchingCubesController< MultiMaterial >::MaterialType > > smoothTerrainMesh;
						MarchingCubesSurfaceExtractor< SimpleVolume<VoxelType> > surfaceExtractor(mVolData, regionToExtract, &smoothTerrainMesh);
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
