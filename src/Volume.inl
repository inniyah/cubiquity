#include "MeshPart.h"

using namespace gameplay;
using namespace PolyVox;

#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Raycast.h"
#include "PolyVoxCore/VolumeResampler.h"

#include "GameplayMarchingCubesController.h"
#include "GameplayIsQuadNeeded.h"

#include "Main.h" //Naughty, but we can temporarily use the define for cubic vs smooth terrain.

#include "CubiquityUtility.h"

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
Volume<VoxelType>::Volume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth, unsigned int baseNodeSize)
	:mVolData(0)
	,mRootOctreeNode(0)
	,mType(type)
	,mRegionWidth(regionWidth)
	,mRegionHeight(regionHeight)
	,mRegionDepth(regionDepth)
	,mBaseNodeSize(baseNodeSize)
	,mTime(0)
{
	Region volumeRegion(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);

	GP_ASSERT(volumeRegion.getWidthInVoxels() > 0);
	GP_ASSERT(volumeRegion.getHeightInVoxels() > 0);
	GP_ASSERT(volumeRegion.getDepthInVoxels() > 0);
	GP_ASSERT(volumeRegion.getWidthInVoxels() % regionWidth == 0);
	GP_ASSERT(volumeRegion.getHeightInVoxels() % regionHeight == 0);
	GP_ASSERT(volumeRegion.getDepthInVoxels() % regionDepth == 0);
	
	mVolData = new RawVolume<VoxelType>(volumeRegion);

	//mRootNode = Node::create();

	GP_ASSERT(isPowerOf2(mBaseNodeSize));

	uint32_t largestVolumeDimensionInVoxels = std::max(volumeRegion.getWidthInVoxels(), std::max(volumeRegion.getHeightInVoxels(), volumeRegion.getDepthInVoxels()));

	uint32_t octreeTargetSizeInVoxels = upperPowerOfTwo(largestVolumeDimensionInVoxels);

	uint32_t widthIncrease = octreeTargetSizeInVoxels - volumeRegion.getWidthInVoxels();
	uint32_t heightIncrease = octreeTargetSizeInVoxels - volumeRegion.getHeightInVoxels();
	uint32_t depthIncrease = octreeTargetSizeInVoxels - volumeRegion.getDepthInVoxels();
	
	if(widthIncrease % 2 == 1)
	{
		upperX++;
		widthIncrease--;
	}

	if(heightIncrease % 2 == 1)
	{
		upperY++;
		heightIncrease--;
	}
	if(depthIncrease % 2 == 1)
	{
		upperZ++;
		depthIncrease--;
	}

	Region octreeRegion(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);
	octreeRegion.grow(widthIncrease / 2, heightIncrease / 2, depthIncrease / 2);

	mRootOctreeNode = new OctreeNode(octreeRegion, 0);

	buildOctreeNodeTree(mRootOctreeNode);

	/*for(int z = 0; z < volumeDepthInRegions; z++)
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

				mOctreeNodes[x][y][z] = new OctreeNode(Region(regLowerX, regLowerY, regLowerZ, regUpperX, regUpperY, regUpperZ), mRootNode);
			}
		}
	}*/
}

template <typename VoxelType>
Volume<VoxelType>::~Volume()
{
}

template <typename VoxelType>
void Volume<VoxelType>::buildOctreeNodeTree(OctreeNode* parent)
{
	GP_ASSERT(parent->mRegion.getWidthInVoxels() == parent->mRegion.getHeightInVoxels());
	GP_ASSERT(parent->mRegion.getHeightInVoxels() == parent->mRegion.getDepthInVoxels());

	if(parent->mRegion.getWidthInVoxels() > mBaseNodeSize)
	{
		Vector3DInt32 baseLowerCorner = parent->mRegion.getLowerCorner();
		int32_t width = parent->mRegion.getWidthInVoxels() / 2;
		int32_t height = parent->mRegion.getHeightInVoxels() / 2;
		int32_t depth = parent->mRegion.getDepthInVoxels() / 2;
		Vector3DInt32 baseUpperCorner = baseLowerCorner + Vector3DInt32(width-1, height-1, depth-1);

		for(int z = 0; z < 2; z++)
		{
			for(int y = 0; y < 2; y++)
			{
				for(int x = 0; x < 2; x++)
				{
					Vector3DInt32 offset (x*width, y*height, z*depth);
					OctreeNode* volReg = new OctreeNode(Region(baseLowerCorner + offset, baseUpperCorner + offset), parent);
					parent->children[x][y][z] = volReg;
					buildOctreeNodeTree(volReg);
				}
			}
		}
	}
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
	mRootOctreeNode->markDataAsModified(x, y, z, getTime());
}

template <typename VoxelType>
void Volume<VoxelType>::update()
{
	updateMesh(mRootOctreeNode);
}

template <typename VoxelType>
void Volume<VoxelType>::updateMesh(OctreeNode* volReg)
{
	if((volReg->isMeshUpToDate() == false) && (volReg->mWantedForRendering))
	{
		Region lod0Region = volReg->mRegion;

		//Extract the surface
		if(getType() == VolumeTypes::ColouredCubes)
		{
			//GameplayIsQuadNeeded<VoxelType> isQuadNeeded;
			SurfaceMesh<PositionMaterial<VoxelType> >* colouredCubicMesh = new SurfaceMesh<PositionMaterial<VoxelType> >;
			//CubicSurfaceExtractor< RawVolume<VoxelType>, GameplayIsQuadNeeded<VoxelType> > surfaceExtractor(mVolData, lod0Region, &colouredCubicMesh, WrapModes::Border, VoxelType(0), true, isQuadNeeded);
			//surfaceExtractor.execute();

			uint32_t downScaleFactor = 0x0001 << (volReg->subtreeHeight() - 1);

			generateCubicMesh(lod0Region, downScaleFactor, colouredCubicMesh);

			if(colouredCubicMesh->getNoOfIndices() > 0)
			{
				volReg->buildGraphicsMesh(colouredCubicMesh/*, 0*/);
			}
		}
		else if(getType() == VolumeTypes::SmoothTerrain)
		{
			SurfaceMesh<PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >* mesh = new SurfaceMesh<PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >;

			uint32_t downScaleFactor = 0x0001 << (volReg->subtreeHeight() - 1);

			generateSmoothMesh(lod0Region, downScaleFactor, mesh);

			if(downScaleFactor > 1)
			{
				recalculateMaterials(mesh, static_cast<Vector3DFloat>(lod0Region.getLowerCorner()), mVolData);
			}

			if(mesh->getNoOfIndices() > 0)
			{
				volReg->buildGraphicsMesh(mesh/*, 0*/);
			}

		}

		volReg->setMeshLastUpdated(getTime());
	}

	for(int z = 0; z < 2; z++)
	{
		for(int y = 0; y < 2; y++)
		{
			for(int x = 0; x < 2; x++)
			{
				OctreeNode* child = volReg->children[x][y][z];
				if(child)
				{
					updateMesh(child);
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
#ifdef TERRAIN_SMOOTH
	/*std::vector< PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >& vertices = mesh->getRawVertexData();
	for(int ct = 0; ct < vertices.size(); ct++)
	{
		const Vector3DFloat& vertexPos = vertices[ct].getPosition() + meshOffset;
		VoxelType value = getInterpolatedValue(volume, vertexPos);

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
	}*/
#endif
}

template <typename VoxelType>
VoxelType Volume<VoxelType>::getInterpolatedValue(RawVolume<VoxelType>* volume, const Vector3DFloat& position)
{
	typename RawVolume<VoxelType>::Sampler sampler(volume);

	int32_t iLowerX = roundTowardsNegInf(position.getX());
	int32_t iLowerY = roundTowardsNegInf(position.getY());
	int32_t iLowerZ = roundTowardsNegInf(position.getZ());

	float fOffsetX = position.getX() - iLowerX;
	float fOffsetY = position.getY() - iLowerY;
	float fOffsetZ = position.getZ() - iLowerZ;

	/*int32_t iCeilX = iFloorX + 1;
	int32_t iCeilY = iFloorY + 1;
	int32_t iCeilZ = iFloorZ + 1;*/

	sampler.setPosition(iLowerX, iLowerY, iLowerZ);

	VoxelType v000 = sampler.peekVoxel0px0py0pz();
	VoxelType v100 = sampler.peekVoxel1px0py0pz();
	VoxelType v010 = sampler.peekVoxel0px1py0pz();
	VoxelType v110 = sampler.peekVoxel1px1py0pz();
	VoxelType v001 = sampler.peekVoxel0px0py1pz();
	VoxelType v101 = sampler.peekVoxel1px0py1pz();
	VoxelType v011 = sampler.peekVoxel0px1py1pz();
	VoxelType v111 = sampler.peekVoxel1px1py1pz();

	VoxelType result = trilerp(v000, v100, v010, v110, v001, v101, v011, v111, fOffsetX, fOffsetY, fOffsetZ);

	return result;
}

template <typename VoxelType>
void Volume<VoxelType>::generateSmoothMesh(const PolyVox::Region& region, uint32_t downSampleFactor, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > >* resultMesh)
{
	GameplayMarchingCubesController<VoxelType> controller;
	if(downSampleFactor == 1)
	{
		//SurfaceMesh<PositionMaterialNormal< typename GameplayMarchingCubesController<VoxelType>::MaterialType > > mesh;
		MarchingCubesSurfaceExtractor< RawVolume<VoxelType>, GameplayMarchingCubesController<VoxelType> > surfaceExtractor(mVolData, region, resultMesh, WrapModes::Clamp, VoxelType(0), controller);
		surfaceExtractor.execute();
	}
	else
	{
		Region lod2Region = region;
		Vector3DInt32 lowerCorner = lod2Region.getLowerCorner();
		Vector3DInt32 upperCorner = lod2Region.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(downSampleFactor);
		upperCorner = upperCorner + lowerCorner;
		lod2Region.setUpperCorner(upperCorner);

		RawVolume<VoxelType> resampledVolume(lod2Region);
		//lod1Volume.m_bClampInsteadOfBorder = true; //We're extracting right to the edge of our small volume, so this keeps the normals correct(ish)
		VolumeResampler< RawVolume<VoxelType>, RawVolume<VoxelType> > volumeResampler(mVolData, region, &resampledVolume, lod2Region);
		volumeResampler.execute();

		MarchingCubesSurfaceExtractor< RawVolume<VoxelType>, GameplayMarchingCubesController<VoxelType> > surfaceExtractor(&resampledVolume, lod2Region, resultMesh, WrapModes::Clamp, VoxelType(0), controller);
		surfaceExtractor.execute();

		resultMesh->scaleVertices(downSampleFactor);
	}
}

template <typename VoxelType>
void Volume<VoxelType>::generateCubicMesh(const PolyVox::Region& region, uint32_t downSampleFactor, PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >* resultMesh)
{
	GameplayIsQuadNeeded<VoxelType> isQuadNeeded;

	if(downSampleFactor != 2) //HACK
	{
		SurfaceMesh<PositionMaterial<VoxelType> > colouredCubicMesh;
		CubicSurfaceExtractor< RawVolume<VoxelType>, GameplayIsQuadNeeded<VoxelType> > surfaceExtractor(mVolData, region, resultMesh, WrapModes::Border, VoxelType(0), true, isQuadNeeded);
		surfaceExtractor.execute();
	}
	else
	{
		Region lod2Region = region;
		Vector3DInt32 lowerCorner = lod2Region.getLowerCorner();
		Vector3DInt32 upperCorner = lod2Region.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(downSampleFactor);
		upperCorner = upperCorner + lowerCorner;
		lod2Region.setUpperCorner(upperCorner);

		RawVolume<VoxelType> resampledVolume(lod2Region);
		//VolumeResampler< RawVolume<VoxelType>, RawVolume<VoxelType> > volumeResampler(mVolData, region, &resampledVolume, lod2Region);
		rescaleCubicVolume(mVolData, region, &resampledVolume, lod2Region);
		//volumeResampler.execute();

		SurfaceMesh<PositionMaterial<VoxelType> > colouredCubicMesh;
		CubicSurfaceExtractor< RawVolume<VoxelType>, GameplayIsQuadNeeded<VoxelType> > surfaceExtractor(&resampledVolume, lod2Region, resultMesh, WrapModes::Border, VoxelType(0), true, isQuadNeeded);
		surfaceExtractor.execute();

		resultMesh->scaleVertices(downSampleFactor);
	}
}