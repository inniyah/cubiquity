#include "ColouredCubesVolume.h"

void ColouredCubesVolume::updateMeshImpl(OctreeNode* volReg)
{
	PolyVox::Region lod0Region = volReg->mRegion;

	//Extract the surface
	if(getType() == VolumeTypes::ColouredCubes)
	{
		//ColouredCubesIsQuadNeeded<VoxelType> isQuadNeeded;
		PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >* colouredCubicMesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterial<VoxelType> >;
		//CubicSurfaceExtractor< RawVolume<VoxelType>, ColouredCubesIsQuadNeeded<VoxelType> > surfaceExtractor(mVolData, lod0Region, &colouredCubicMesh, WrapModes::Border, VoxelType(0), true, isQuadNeeded);
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
		PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >* mesh = new PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal< typename MultiMaterialMarchingCubesController<VoxelType>::MaterialType > >;

		uint32_t downScaleFactor = 0x0001 << (volReg->subtreeHeight() - 1);

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
}