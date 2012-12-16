#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Raycast.h"
#include "PolyVoxCore/VolumeResampler.h"

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
class RaycastTestFunctor<MultiMaterial4>
{
public:
	RaycastTestFunctor()
	{
	}

	bool operator()(Vector3DFloat pos, const MultiMaterial4& voxel)
	{
		mLastPos = pos;
		return voxel.getSumOfMaterials() <= MultiMaterial4::getMaxMaterialValue() / 2;
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

// Note: This function is not implemented in a very efficient manner and it rather slow.
// A better implementation should make use of the 'peek' functions to sample the voxel data,
// but this will require careful handling of the cases when the ray is outside the volume.
// It could also compute entry and exit points to avoid having to test every step for whether
// it is still inside the volume.
// Also, should we handle computing the exact intersection point? Repeatedly bisect the last
// two points, of perform interpolation between them? Maybe user code could perform such interpolation?
template<typename VolumeType, typename Callback>
RaycastResult smoothRaycastWithDirection(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirectionAndLength, Callback& callback, float fStepSize = 1.0f)
{		
	int mMaxNoOfSteps = v3dDirectionAndLength.length() / fStepSize;

	Vector3DFloat v3dPos = v3dStart;
	const Vector3DFloat v3dStep =  v3dDirectionAndLength / static_cast<float>(mMaxNoOfSteps);

	for(uint32_t ct = 0; ct < mMaxNoOfSteps; ct++)
	{
		float fPosX = v3dPos.getX();
		float fPosY = v3dPos.getY();
		float fPosZ = v3dPos.getZ();

		float fFloorX = floor(fPosX);
		float fFloorY = floor(fPosY);
		float fFloorZ = floor(fPosZ);

		float fInterpX = fPosX - fFloorX;
		float fInterpY = fPosY - fFloorY;
		float fInterpZ = fPosZ - fFloorZ;

		// Conditional logic required to round negative floats correctly
		int32_t iX = static_cast<int32_t>(fFloorX > 0.0f ? fFloorX + 0.5f : fFloorX - 0.5f); 
		int32_t iY = static_cast<int32_t>(fFloorY > 0.0f ? fFloorY + 0.5f : fFloorY - 0.5f); 
		int32_t iZ = static_cast<int32_t>(fFloorZ > 0.0f ? fFloorZ + 0.5f : fFloorZ - 0.5f);

		const typename VolumeType::VoxelType& voxel000 = volData->getVoxelAt(iX, iY, iZ);
		const typename VolumeType::VoxelType& voxel001 = volData->getVoxelAt(iX, iY, iZ + 1);
		const typename VolumeType::VoxelType& voxel010 = volData->getVoxelAt(iX, iY + 1, iZ);
		const typename VolumeType::VoxelType& voxel011 = volData->getVoxelAt(iX, iY + 1, iZ + 1);
		const typename VolumeType::VoxelType& voxel100 = volData->getVoxelAt(iX + 1, iY, iZ);
		const typename VolumeType::VoxelType& voxel101 = volData->getVoxelAt(iX + 1, iY, iZ + 1);
		const typename VolumeType::VoxelType& voxel110 = volData->getVoxelAt(iX + 1, iY + 1, iZ);
		const typename VolumeType::VoxelType& voxel111 = volData->getVoxelAt(iX + 1, iY + 1, iZ + 1);

		typename VolumeType::VoxelType tInterpolatedValue = trilerp(voxel000,voxel100,voxel010,voxel110,voxel001,voxel101,voxel011,voxel111,fInterpX,fInterpY,fInterpZ);
		
		if(!callback(v3dPos, tInterpolatedValue))
		{
			return RaycastResults::Interupted;
		}

		v3dPos += v3dStep;
	}

	return RaycastResults::Completed;
}

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
	mVolData = new RawVolume<VoxelType>(Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ));

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
				// get closed, so we want to mimic this behaviour on the lower edges too.
				if(getType() == VolumeTypes::SmoothTerrain)
				{
					//We only need to subtract 1 for highest LOD, but subtract 4 to allow 3 LOD levels.
					//if(x == 0) regLowerX -= 4;
					//if(y == 0) regLowerY -= 4;
					//if(z == 0) regLowerZ -= 4;
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

				mVolumeRegions[x][y][z] = new VolumeRegion(Region(regLowerX, regLowerY, regLowerZ, regUpperX, regUpperY, regUpperZ), mRootNode);
			}
		}
	}
}

template <typename VoxelType>
Volume<VoxelType>::~Volume()
{
	for(int z = 0; z < mVolumeRegions.getDimension(2); z++)
	{
		for(int y = 0; y < mVolumeRegions.getDimension(1); y++)
		{
			for(int x = 0; x < mVolumeRegions.getDimension(0); x++)
			{
				delete mVolumeRegions[x][y][z];
			}
		}
	}
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
					Region lod0Region = mVolumeRegions[x][y][z]->mRegion;
					//Extract the surface
					if(getType() == VolumeTypes::ColouredCubes)
					{
						GameplayIsQuadNeeded<VoxelType> isQuadNeeded;
						SurfaceMesh<PositionMaterial<VoxelType> > colouredCubicMesh;
						CubicSurfaceExtractor< RawVolume<VoxelType>, GameplayIsQuadNeeded<VoxelType> > surfaceExtractor(mVolData, lod0Region, &colouredCubicMesh, true, isQuadNeeded);
						surfaceExtractor.execute();

						if(colouredCubicMesh.getNoOfIndices() > 0)
						{
							mVolumeRegions[x][y][z]->buildGraphicsMesh(colouredCubicMesh, 0);
						}
					}
					else if(getType() == VolumeTypes::SmoothTerrain)
					{
						GameplayMarchingCubesController<VoxelType> controller;
						controller.setWrapMode(WrapModes::Clamp);
						SurfaceMesh<PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > > lod0Mesh;
						MarchingCubesSurfaceExtractor< RawVolume<VoxelType>, GameplayMarchingCubesController<VoxelType> > surfaceExtractor(mVolData, lod0Region, &lod0Mesh, controller);
						surfaceExtractor.execute();

						if(lod0Mesh.getNoOfIndices() > 0)
						{
							mVolumeRegions[x][y][z]->buildGraphicsMesh(lod0Mesh, 0);
						}

						// I'm having a lot of difficulty getting the lod levels to work properly. Say I have a region of 17x17x17 voxels
						// at the highest lod... that 16x16x16 cells. I currently downsample this to 9x9x9 voxels (8x8x8 cells) and then
						// run the MC algorithm again on this region of 9x9x9 voxels. But to make the tiles overlap I'd like to actually 
						// run it on an 11x11x11 region which extends outside the downsampled volume. The downsampled volume should be clamped
						// (I think) and this doesn't seem to be working properly with the marching cubes.
						//
						// I can see it would make more sense if the Volume::Sampler handled the wrap mode rather than the Volume (which should
						// then have no concept of borders, etc). This is more flexible as the same volume can then also be sampled in different
						// ways, as well as allowing algorithms which use the sampler (such as MC) to properly honour the wrap mode. I will come
						// back and look at this code again after I make these changes in PolyVox.
						//
						// Actually, I'm now concluding that the lower LOD volume data does need to overlap for better continuity. Otherwise 
						// we're seeing cracks een between meshes of the same LOD level.

						Region lod1VolumeRegion(lod0Region);	
						Vector3DInt32 lowerCorner = lod1VolumeRegion.getLowerCorner();
						Vector3DInt32 upperCorner = lod1VolumeRegion.getUpperCorner();

						upperCorner = upperCorner - lowerCorner;
						upperCorner = upperCorner / 2;
						upperCorner = upperCorner + lowerCorner;
						lod1VolumeRegion.setUpperCorner(upperCorner);

						RawVolume<VoxelType> lod1Volume(lod1VolumeRegion);
						//lod1Volume.m_bClampInsteadOfBorder = true; //We're extracting right to the edge of our small volume, so this keeps the normals correct(ish)
						VolumeResampler< RawVolume<VoxelType>, RawVolume<VoxelType> > volumeResampler(mVolData, lod0Region, &lod1Volume, lod1VolumeRegion);
						volumeResampler.execute();

						Region lod1MeshRegion(lod1VolumeRegion);
						//lod1MeshRegion.shiftUpperCorner(Vector3DInt32( 1, 1, 1));
						//lod1MeshRegion.shiftLowerCorner(Vector3DInt32(-1,-1,-1));

						SurfaceMesh<PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > > lod1Mesh;
						MarchingCubesSurfaceExtractor< RawVolume<VoxelType>, GameplayMarchingCubesController<VoxelType> > surfaceExtractor2(&lod1Volume, lod1MeshRegion, &lod1Mesh, controller);
						surfaceExtractor2.execute();

						//lod1Mesh.translateVertices(Vector3DFloat(-1.0f, -1.0f, -1.0f));
						lod1Mesh.scaleVertices(2.0f);

						recalculateMaterials(&lod1Mesh, static_cast<Vector3DFloat>(lod0Region.getLowerCorner()), mVolData);
						

						if(lod1Mesh.getNoOfIndices() > 0)
						{
							mVolumeRegions[x][y][z]->buildGraphicsMesh(lod1Mesh, 1);
						}

						Region lod2VolumeRegion(lod1VolumeRegion);	
						lowerCorner = lod2VolumeRegion.getLowerCorner();
						upperCorner = lod2VolumeRegion.getUpperCorner();

						upperCorner = upperCorner - lowerCorner;
						upperCorner = upperCorner / 2;
						upperCorner = upperCorner + lowerCorner;
						lod2VolumeRegion.setUpperCorner(upperCorner);

						RawVolume<VoxelType> lod2Volume(lod2VolumeRegion);
						//lod2Volume.m_bClampInsteadOfBorder = true; //We're extracting right to the edge of our small volume, so this keeps the normals correct(ish)
						VolumeResampler< RawVolume<VoxelType>, RawVolume<VoxelType> > volumeResampler2(&lod1Volume, lod1VolumeRegion, &lod2Volume, lod2VolumeRegion);
						volumeResampler2.execute();

						Region lod2MeshRegion(lod2VolumeRegion);
						//lod2MeshRegion.shiftUpperCorner(Vector3DInt32( 1, 1, 1));
						//lod2MeshRegion.shiftLowerCorner(Vector3DInt32(-1,-1,-1));

						SurfaceMesh<PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > > lod2Mesh;
						MarchingCubesSurfaceExtractor< RawVolume<VoxelType>, GameplayMarchingCubesController<VoxelType> > surfaceExtractor3(&lod2Volume, lod2MeshRegion, &lod2Mesh, controller);
						surfaceExtractor3.execute();

						//lod2Mesh.translateVertices(Vector3DFloat(-1.0f, -1.0f, -1.0f));
						lod2Mesh.scaleVertices(4.0f);

						recalculateMaterials(&lod2Mesh, static_cast<Vector3DFloat>(lod0Region.getLowerCorner()), mVolData);

						if(lod1Mesh.getNoOfIndices() > 0)
						{
							mVolumeRegions[x][y][z]->buildGraphicsMesh(lod2Mesh, 2);
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

template <typename VoxelType>
void Volume<VoxelType>::recalculateMaterials(SurfaceMesh<PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >* mesh, const Vector3DFloat& meshOffset,  RawVolume<VoxelType>* volume)
{
	std::vector< PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >& vertices = mesh->getRawVertexData();
	for(int ct = 0; ct < vertices.size(); ct++)
	{
		const Vector3DFloat& vertexPos = vertices[ct].getPosition() + meshOffset;
		VoxelType value = volume->getInterpolatedValue(vertexPos);

		// It seems that sometimes the vertices can fall in an empty cell. The reason for this
		// isn't clear but it might be inaccuraceies in the lower LOD mesh. It also might only 
		// happen right on the edge of the volume so wrap modes might help. Hopefully we can
		// remove this hack in the future.
		Vector<4, float> matAsVec = value;
		if(matAsVec.length() < 0.001f)
		{
			value = VoxelType(0);
			value.setMaterial(0, 255);
		}

		vertices[ct].setMaterial(value);
	}
}