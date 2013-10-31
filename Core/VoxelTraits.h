#ifndef CUBIQUITY_VOXELTRAITS_H_
#define CUBIQUITY_VOXELTRAITS_H_

#include "MaterialSet.h"

namespace Cubiquity
{
	// We use traits to decide (for example) which vertex type should correspond to a given voxel type, 
	// or which surface extractor should be used for a given voxel type. Maybe it is useful to consider
	// putting some of this (the VoxelType to VertexType maybe?) into PolyVox.
	template<typename Type>
	class VoxelTraits;

	template<>
	class VoxelTraits<Colour>
	{
	public:
		typedef ::PolyVox::PositionMaterial<Colour> VertexType;
		typedef ColouredCubicSurfaceExtractionTask SurfaceExtractionTaskType;
		static const bool IsColour = true;
		static const bool IsMaterialSet = false;
	};

	template<>
	class VoxelTraits<MaterialSet>
	{
	public:
		typedef ::PolyVox::PositionMaterialNormal< MaterialSetMarchingCubesController::MaterialType > VertexType;
		typedef SmoothSurfaceExtractionTask SurfaceExtractionTaskType;
		static const bool IsColour = false;
		static const bool IsMaterialSet = true;
	};
}

#endif //CUBIQUITY_VOXELTRAITS_H_