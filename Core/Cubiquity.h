#ifndef CUBIQUITY_MAIN_HEADER_H_
#define CUBIQUITY_MAIN_HEADER_H_

#include "Colour.h"
#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class ColouredCubesVolume
	{
	public:
		virtual ~ColouredCubesVolume() {};

		virtual int32_t getLowerX(void) const = 0;
		virtual int32_t getUpperX(void) const = 0;
		virtual int32_t getLowerY(void) const = 0;
		virtual int32_t getUpperY(void) const = 0;
		virtual int32_t getLowerZ(void) const = 0;
		virtual int32_t getUpperZ(void) const = 0;

		virtual uint32_t getWidth(void) const = 0;
		virtual uint32_t getHeight(void) const = 0;
		virtual uint32_t getDepth(void) const = 0;

		// Note this adds a border rather than calling straight through.
		virtual Colour getVoxelAt(int32_t x, int32_t y, int32_t z) const = 0;

		// Octree access
		virtual Octree<Colour>* getOctree(void) = 0;
		virtual OctreeNode<Colour>* getRootOctreeNode(void) = 0;

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		virtual void setVoxelAt(int32_t x, int32_t y, int32_t z, Colour value, UpdatePriority updatePriority = UpdatePriorities::Background) = 0;

		// Marks a region as modified so it will be regenerated later.
		virtual void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background) = 0;

		virtual void acceptOverrideBlocks(void) = 0;
		virtual void discardOverrideBlocks(void) = 0;

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold) = 0;
	};

	class SmoothTerrainVolume
	{
	public:
		virtual ~SmoothTerrainVolume() {};

		virtual int32_t getLowerX(void) const = 0;
		virtual int32_t getUpperX(void) const = 0;
		virtual int32_t getLowerY(void) const = 0;
		virtual int32_t getUpperY(void) const = 0;
		virtual int32_t getLowerZ(void) const = 0;
		virtual int32_t getUpperZ(void) const = 0;

		virtual uint32_t getWidth(void) const = 0;
		virtual uint32_t getHeight(void) const = 0;
		virtual uint32_t getDepth(void) const = 0;

		// Note this adds a border rather than calling straight through.
		virtual MultiMaterial getVoxelAt(int32_t x, int32_t y, int32_t z) const = 0;

		// Octree access
		virtual Octree<MultiMaterial>* getOctree(void) = 0;
		virtual OctreeNode<MultiMaterial>* getRootOctreeNode(void) = 0;

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		virtual void setVoxelAt(int32_t x, int32_t y, int32_t z, MultiMaterial value, UpdatePriority updatePriority = UpdatePriorities::Background) = 0;

		// Marks a region as modified so it will be regenerated later.
		virtual void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background) = 0;

		virtual void acceptOverrideBlocks(void) = 0;
		virtual void discardOverrideBlocks(void) = 0;

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold) = 0;
	};
}

#endif
