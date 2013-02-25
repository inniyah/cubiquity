#ifndef CUBIQUITY_VOXELTRAITS_H_
#define CUBIQUITY_VOXELTRAITS_H_

#include "MultiMaterial.h"

// We use traits to decide (for example) which vertex type should correspond to a given voxel type, 
// or which surface extractor should be used for a given voxel type. Maybe it is useful to consider
// putting some of this (the VoxelType to VertexType maybe?) into PolyVox.
template<typename Type>
class VoxelTraits;

template<>
class VoxelTraits<Colour>
{
public:
	typedef PolyVox::PositionMaterial<Colour> VertexType;
	typedef ColouredCubicSurfaceExtractionTask SurfaceExtractionTaskType;
};

template<>
class VoxelTraits<MultiMaterial>
{
public:
	typedef PolyVox::PositionMaterialNormal< MultiMaterialMarchingCubesController::MaterialType > VertexType;
	typedef SmoothSurfaceExtractionTask SurfaceExtractionTaskType;
};

#endif //CUBIQUITY_VOXELTRAITS_H_