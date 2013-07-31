#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "Colour.h"
#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class ColouredCubesVolume
	{
	public:
		//Hack!
		typedef Colour VoxelType;

		// These functions just forward to the underlying PolyVox volume.
		virtual uint32_t getWidth(void) const = 0;
		virtual uint32_t getHeight(void) const = 0;
		virtual uint32_t getDepth(void) const = 0;
		virtual const Region& getEnclosingRegion(void) const = 0;

		// Note this adds a border rather than calling straight through.
		virtual Colour getVoxelAt(int32_t x, int32_t y, int32_t z) const = 0;

		// Octree access
		virtual Octree<Colour>* getOctree(void) = 0;
		virtual OctreeNode<Colour>* getRootOctreeNode(void) = 0;

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		virtual void setVoxelAt(int32_t x, int32_t y, int32_t z, Colour value, UpdatePriority updatePriority = UpdatePriorities::Background) = 0;

		// Marks a region as mdified so it will be regenerated later.
		virtual void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background) = 0;

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold) = 0;

		//HACK!
		virtual ::PolyVox::POLYVOX_VOLUME<Colour>* _getPolyVoxVolume(void) = 0;
	};

	class ColouredCubesVolumeImpl : public ColouredCubesVolume
	{
	public:
		ColouredCubesVolumeImpl(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize)
			:mCubiquityVolume(region, pageFolder, OctreeConstructionModes::BoundVoxels, baseNodeSize) {}

		// These functions just forward to the underlying PolyVox volume.
		virtual uint32_t getWidth(void) const {return mCubiquityVolume.getWidth();}
		virtual uint32_t getHeight(void) const {return mCubiquityVolume.getHeight();}
		virtual uint32_t getDepth(void) const {return mCubiquityVolume.getDepth();}
		virtual const Region& getEnclosingRegion(void) const {return mCubiquityVolume.getEnclosingRegion();}

		// Note this adds a border rather than calling straight through.
		virtual Colour getVoxelAt(int32_t x, int32_t y, int32_t z) const {return mCubiquityVolume.getVoxelAt(x, y, z);}

		// Octree access
		virtual Octree<Colour>* getOctree(void) {return mCubiquityVolume.getOctree();}
		virtual OctreeNode<Colour>* getRootOctreeNode(void) {return mCubiquityVolume.getRootOctreeNode();}

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		virtual void setVoxelAt(int32_t x, int32_t y, int32_t z, Colour value, UpdatePriority updatePriority = UpdatePriorities::Background) {mCubiquityVolume.setVoxelAt(x, y, z, value, updatePriority); }

		// Marks a region as mdified so it will be regenerated later.
		virtual void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background) {mCubiquityVolume.markAsModified(region, updatePriority);}

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold) {mCubiquityVolume.update(viewPosition, lodThreshold);}

		//HACK!
		virtual ::PolyVox::POLYVOX_VOLUME<Colour>* _getPolyVoxVolume(void) {return getPolyVoxVolumeFrom<Colour>(&mCubiquityVolume); }

	private:
		Volume<Colour> mCubiquityVolume;
	};

	ColouredCubesVolume* createColoredCubesVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize);
}

#endif //COLOUREDCUBESVOLUME_H_
