#ifndef __Cubiquity_ForwardDeclarations_H__
#define __Cubiquity_ForwardDeclarations_H__

#include "PolyVoxCore/PolyVoxForwardDeclarations.h"

namespace Cubiquity
{
	class Brush;
	class Clock;

	class Color;

	class ColoredCubesIsQuadNeeded;

	class ColoredCubesVolume;
	class ColoredCubesVolumeImpl;

	class ColoredCubicSurfaceExtractionTask;

	class MainThreadTaskProcessor;

	class MaterialSet;

	class MaterialSetMarchingCubesController;

	template <typename VoxelType>
	class Octree;

	template <typename VoxelType>
	class OctreeNode;

	template <typename VoxelType>
	class OverrideFilePager;

	class SmoothSurfaceExtractionTask;

	class TerrainVolume;
	class TerrainVolumeImpl;

	class TerrainVolumeEditor;

	template <typename VoxelType>
	class SQLitePager;

	class Task;
	class TaskProcessor;

	template <typename VoxelType>
	class Volume;
}

#endif //__Cubiquity_ForwardDeclarations_H__
