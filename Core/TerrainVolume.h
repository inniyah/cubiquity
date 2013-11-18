#ifndef TERRAINVOLUME_H_
#define TERRAINVOLUME_H_

#include "Cubiquity.h"
#include "CubiquityForwardDeclarations.h"
#include "Volume.h"

namespace Cubiquity
{
	class TerrainVolumeImpl : public TerrainVolume
	{
	public:
		typedef MaterialSet VoxelType;

		TerrainVolumeImpl(const Region& region, const std::string& filename, unsigned int baseNodeSize)
			:mCubiquityVolume(region, filename, OctreeConstructionModes::BoundCells, baseNodeSize)
		{
		}

		virtual ~TerrainVolumeImpl() {}

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
		virtual MaterialSet getVoxelAt(int32_t x, int32_t y, int32_t z) const {return mCubiquityVolume.getVoxelAt(x, y, z);}

		// Octree access
		virtual Octree<MaterialSet>* getOctree(void) {return mCubiquityVolume.getOctree();}
		virtual OctreeNode<MaterialSet>* getRootOctreeNode(void) {return mCubiquityVolume.getRootOctreeNode();}

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		virtual void setVoxelAt(int32_t x, int32_t y, int32_t z, MaterialSet value, UpdatePriority updatePriority = UpdatePriorities::Background) {mCubiquityVolume.setVoxelAt(x, y, z, value, updatePriority); }

		// Marks a region as modified so it will be regenerated later.
		virtual void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background) {mCubiquityVolume.markAsModified(region, updatePriority);}

		virtual void acceptOverrideBlocks(void) {mCubiquityVolume.acceptOverrideBlocks(); }
		virtual void discardOverrideBlocks(void) {mCubiquityVolume.discardOverrideBlocks(); }

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold) {mCubiquityVolume.update(viewPosition, lodThreshold);}

		// This one's a bit of a hack... direct access to underlying PolyVox volume
		virtual ::PolyVox::LargeVolume<MaterialSet>* _getPolyVoxVolume(void) {return mCubiquityVolume._getPolyVoxVolume(); }

	public:
		Volume<MaterialSet> mCubiquityVolume;
	};

	TerrainVolume* createTerrainVolume(const Region& region, const std::string& filename, unsigned int baseNodeSize, bool createFloor = true, uint32_t floorDepth = 8);
}

#endif //TERRAINVOLUME_H_
