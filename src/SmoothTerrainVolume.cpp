#include "SmoothTerrainVolume.h"

#include <algorithm>

using namespace std;
using namespace PolyVox;

SmoothTerrainVolume::SmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int baseNodeSize)
	:Volume<MultiMaterial4>(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, OctreeConstructionModes::BoundCells, baseNodeSize)
{
}


void SmoothTerrainVolume::updateMeshImpl(OctreeNode* volReg)
{
	PolyVox::Region lod0Region = volReg->mRegion;

	//Extract the surface
	PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >* mesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >;

	uint32_t downScaleFactor = 0x0001 << volReg->mLodLevel;

	generateSmoothMesh(lod0Region, downScaleFactor, mesh);

	if(downScaleFactor > 1)
	{
		recalculateMaterials(mesh, static_cast<PolyVox::Vector3DFloat>(lod0Region.getLowerCorner()), mVolData);
	}

	if(mesh->getNoOfIndices() > 0)
	{
		volReg->buildGraphicsMesh(mesh/*, 0*/);
	}
}

void SmoothTerrainVolume::generateSmoothMesh(const PolyVox::Region& region, uint32_t downSampleFactor, PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >* resultMesh)
{
	MultiMaterialMarchingCubesController<VoxelType> controller;
	if(downSampleFactor == 1)
	{
		//SurfaceMesh<PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > > mesh;
		PolyVox::MarchingCubesSurfaceExtractor< PolyVox::RawVolume<VoxelType>, MultiMaterialMarchingCubesController<VoxelType> > surfaceExtractor(mVolData, region, resultMesh, PolyVox::WrapModes::Border, VoxelType(0), controller);
		surfaceExtractor.execute();
	}
	else
	{
		PolyVox::Region highRegion = region;
		highRegion.grow(downSampleFactor, downSampleFactor, downSampleFactor);

		PolyVox::Region lowRegion = highRegion;
		PolyVox::Vector3DInt32 lowerCorner = lowRegion.getLowerCorner();
		PolyVox::Vector3DInt32 upperCorner = lowRegion.getUpperCorner();

		upperCorner = upperCorner - lowerCorner;
		upperCorner = upperCorner / static_cast<int32_t>(downSampleFactor);
		upperCorner = upperCorner + lowerCorner;
		lowRegion.setUpperCorner(upperCorner);

		PolyVox::RawVolume<VoxelType> resampledVolume(lowRegion);
		//lod1Volume.m_bClampInsteadOfBorder = true; //We're extracting right to the edge of our small volume, so this keeps the normals correct(ish)
		PolyVox::VolumeResampler< PolyVox::RawVolume<VoxelType>, PolyVox::RawVolume<VoxelType> > volumeResampler(mVolData, highRegion, &resampledVolume, lowRegion);
		volumeResampler.execute();

		lowRegion.shrink(1, 1, 1);

		PolyVox::MarchingCubesSurfaceExtractor< PolyVox::RawVolume<VoxelType>, MultiMaterialMarchingCubesController<VoxelType> > surfaceExtractor(&resampledVolume, lowRegion, resultMesh, PolyVox::WrapModes::Border, VoxelType(0), controller);
		surfaceExtractor.execute();

		resultMesh->scaleVertices(downSampleFactor);
	}
}

void SmoothTerrainVolume::recalculateMaterials(PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<MultiMaterial4>::MaterialType > >* mesh, const PolyVox::Vector3DFloat& meshOffset,  PolyVox::RawVolume<MultiMaterial4>* volume)
{
	std::vector< PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >& vertices = mesh->getRawVertexData();
	for(int ct = 0; ct < vertices.size(); ct++)
	{
		const Vector3DFloat& vertexPos = vertices[ct].getPosition() + meshOffset;
		MultiMaterial4 value = getInterpolatedValue(volume, vertexPos);

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

MultiMaterial4 SmoothTerrainVolume::getInterpolatedValue(PolyVox::RawVolume<MultiMaterial4>* volume, const PolyVox::Vector3DFloat& position)
{
	PolyVox::RawVolume<MultiMaterial4>::Sampler sampler(volume);

	int32_t iLowerX = PolyVox::roundTowardsNegInf(position.getX());
	int32_t iLowerY = PolyVox::roundTowardsNegInf(position.getY());
	int32_t iLowerZ = PolyVox::roundTowardsNegInf(position.getZ());

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