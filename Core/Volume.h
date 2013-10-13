#ifndef VOLUME_H_
#define VOLUME_H_

#include "CubiquityForwardDeclarations.h"
#include "Octree.h"
#include "UpdatePriorities.h"
#include "Vector.h"
#include "VoxelTraits.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"

#ifdef USE_LARGE_VOLUME
#include "PolyVoxCore/LargeVolume.h"
#else
#include "PolyVoxCore/SimpleVolume.h"
#endif


#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

namespace Cubiquity
{
	template <typename _VoxelType>
	class Volume
	{
	public:
		typedef _VoxelType VoxelType;

		Volume(const Region& region, const std::string& pageFolder, OctreeConstructionMode octreeConstructionMode, uint32_t baseNodeSize);
		~Volume();

		// These functions just forward to the underlying PolyVox volume.
		uint32_t getWidth(void) const { return mPolyVoxVolume->getWidth(); }
		uint32_t getHeight(void) const { return mPolyVoxVolume->getHeight(); }
		uint32_t getDepth(void) const { return mPolyVoxVolume->getDepth(); }
		const Region& getEnclosingRegion(void) const { return mPolyVoxVolume->getEnclosingRegion(); }

		// Note this adds a border rather than calling straight through.
		VoxelType getVoxelAt(int32_t x, int32_t y, int32_t z) const;

		// This one's a bit of a hack... direct access to underlying PolyVox volume
		::PolyVox::POLYVOX_VOLUME<VoxelType>* _getPolyVoxVolume(void) const { return mPolyVoxVolume; }

		// Octree access
		Octree<VoxelType>* getOctree(void) { return mOctree; };
		OctreeNode<VoxelType>* getRootOctreeNode(void) { return mOctree->getRootNode(); }

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		void setVoxelAt(int32_t x, int32_t y, int32_t z, VoxelType value, UpdatePriority updatePriority = UpdatePriorities::Background);

		// Marks a region as modified so it will be regenerated later.
		void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background);

		void acceptOverrideBlocks(void) { m_pSQLitePager->acceptOverrideBlocks(); }
		void discardOverrideBlocks(void) { m_pSQLitePager->discardOverrideBlocks(); }

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold);

	private:
		Volume& operator=(const Volume&);

		::PolyVox::POLYVOX_VOLUME<VoxelType>* mPolyVoxVolume;

#ifdef USE_LARGE_VOLUME
		::PolyVox::MinizBlockCompressor<VoxelType>* m_pCompressor;
		OverrideFilePager<VoxelType>* m_pOverrideFilePager;
		SQLitePager<VoxelType>* m_pSQLitePager;
#endif

		Octree<VoxelType>* mOctree;

		// Friend functions
		friend class Octree<VoxelType>;
		friend class OctreeNode<VoxelType>;
	};
}

#include "Volume.inl"

#endif //VOLUME_H_
