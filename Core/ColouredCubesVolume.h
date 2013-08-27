#ifndef COLOUREDCUBESVOLUME_H_
#define COLOUREDCUBESVOLUME_H_

#include "Colour.h"
#include "Cubiquity.h"
#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class ColouredCubesVolumeImpl : public ColouredCubesVolume
	{
	public:
		typedef Colour VoxelType;

		ColouredCubesVolumeImpl(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize)
			:mCubiquityVolume(region, pageFolder, OctreeConstructionModes::BoundVoxels, baseNodeSize)
		{
			// Throw an exception if the requested volume dimensions exceed those allowed by the license. Actually the
			// volume has already been constructed at this point, so we're just destroying it again. Might be nice if
			// we could prvent it being constructed in the first place but it's not obvious where such logic would go.
			int32_t maxVolumeSize = 256;
			POLYVOX_THROW_IF(region.getWidthInVoxels() > maxVolumeSize, std::invalid_argument, "Volume width exceeds maximum permitted size");
			POLYVOX_THROW_IF(region.getHeightInVoxels() > maxVolumeSize, std::invalid_argument, "Volume width exceeds maximum permitted size");
			POLYVOX_THROW_IF(region.getDepthInVoxels() > maxVolumeSize, std::invalid_argument, "Volume width exceeds maximum permitted size");
		}

		virtual ~ColouredCubesVolumeImpl() {}

		virtual int32_t getLowerX(void) const { return mCubiquityVolume.getEnclosingRegion().getLowerX(); }
		virtual int32_t getUpperX(void) const { return mCubiquityVolume.getEnclosingRegion().getUpperX(); }
		virtual int32_t getLowerY(void) const { return mCubiquityVolume.getEnclosingRegion().getLowerY(); }
		virtual int32_t getUpperY(void) const { return mCubiquityVolume.getEnclosingRegion().getUpperY(); }
		virtual int32_t getLowerZ(void) const { return mCubiquityVolume.getEnclosingRegion().getLowerZ(); }
		virtual int32_t getUpperZ(void) const { return mCubiquityVolume.getEnclosingRegion().getUpperZ(); }

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

		// Marks a region as modified so it will be regenerated later.
		virtual void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background) {mCubiquityVolume.markAsModified(region, updatePriority);}

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold) {mCubiquityVolume.update(viewPosition, lodThreshold);}

		// This one's a bit of a hack... direct access to underlying PolyVox volume
		virtual ::PolyVox::POLYVOX_VOLUME<Colour>* _getPolyVoxVolume(void) {return mCubiquityVolume._getPolyVoxVolume(); }

	private:
		Volume<Colour> mCubiquityVolume;
	};

	ColouredCubesVolume* createColoredCubesVolume(const Region& region, const std::string& pageFolder, unsigned int baseNodeSize);
}

#endif //COLOUREDCUBESVOLUME_H_
