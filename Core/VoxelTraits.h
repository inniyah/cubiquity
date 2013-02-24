#ifndef CUBIQUITY_VOXELTRAITS_H_
#define CUBIQUITY_VOXELTRAITS_H_

#include "MultiMaterialMarchingCubesController.h"

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
};

// These trait definition should probably be moved to the voxel classes, but at the moment I'm hitting a cyclic
// dependancy with the headers. It might be easier to resolve once MultiMaerial is no longer templatised.
template<>
class VoxelTraits<MultiMaterial>
{
public:
	typedef PolyVox::PositionMaterialNormal< MultiMaterialMarchingCubesController< MultiMaterial >::MaterialType > VertexType;
};

#endif //CUBIQUITY_VOXELTRAITS_H_