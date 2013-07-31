#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class SmoothTerrainVolume
	{
	public:
		//Hack!
		typedef MultiMaterial VoxelType;

		// These functions just forward to the underlying PolyVox volume.
		virtual uint32_t getWidth(void) const = 0;
		virtual uint32_t getHeight(void) const = 0;
		virtual uint32_t getDepth(void) const = 0;
		virtual const Region& getEnclosingRegion(void) const = 0;

		// Note this adds a border rather than calling straight through.
		virtual MultiMaterial getVoxelAt(int32_t x, int32_t y, int32_t z) const = 0;

		// Octree access
		virtual Octree<MultiMaterial>* getOctree(void) = 0;
		virtual OctreeNode<MultiMaterial>* getRootOctreeNode(void) = 0;

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		virtual void setVoxelAt(int32_t x, int32_t y, int32_t z, MultiMaterial value, UpdatePriority updatePriority = UpdatePriorities::Background) = 0;

		// Marks a region as mdified so it will be regenerated later.
		virtual void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background) = 0;

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold) = 0;

		//HACK!
		virtual ::PolyVox::POLYVOX_VOLUME<MultiMaterial>* _getPolyVoxVolume(void) = 0;
	};

	class SmoothTerrainVolumeImpl : public SmoothTerrainVolume
	{
	public:
		SmoothTerrainVolumeImpl(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize)
			:mCubiquityVolume(region, pageFolder, OctreeConstructionModes::BoundCells, baseNodeSize) {}

		// These functions just forward to the underlying PolyVox volume.
		virtual uint32_t getWidth(void) const {return mCubiquityVolume.getWidth();}
		virtual uint32_t getHeight(void) const {return mCubiquityVolume.getHeight();}
		virtual uint32_t getDepth(void) const {return mCubiquityVolume.getDepth();}
		virtual const Region& getEnclosingRegion(void) const {return mCubiquityVolume.getEnclosingRegion();}

		// Note this adds a border rather than calling straight through.
		virtual MultiMaterial getVoxelAt(int32_t x, int32_t y, int32_t z) const {return mCubiquityVolume.getVoxelAt(x, y, z);}

		// Octree access
		virtual Octree<MultiMaterial>* getOctree(void) {return mCubiquityVolume.getOctree();}
		virtual OctreeNode<MultiMaterial>* getRootOctreeNode(void) {return mCubiquityVolume.getRootOctreeNode();}

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		virtual void setVoxelAt(int32_t x, int32_t y, int32_t z, MultiMaterial value, UpdatePriority updatePriority = UpdatePriorities::Background) {mCubiquityVolume.setVoxelAt(x, y, z, value, updatePriority); }

		// Marks a region as mdified so it will be regenerated later.
		virtual void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background) {mCubiquityVolume.markAsModified(region, updatePriority);}

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold) {mCubiquityVolume.update(viewPosition, lodThreshold);}

		//HACK!
		virtual ::PolyVox::POLYVOX_VOLUME<MultiMaterial>* _getPolyVoxVolume(void) {return getPolyVoxVolumeFrom<MultiMaterial>(&mCubiquityVolume); }

	public:
		Volume<MultiMaterial> mCubiquityVolume;
	};

	SmoothTerrainVolume* createSmoothTerrainVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize);
}

#endif //SMOOTHTERRAINVOLUME_H_
