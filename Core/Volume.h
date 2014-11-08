#ifndef VOLUME_H_
#define VOLUME_H_

#include "CubiquityForwardDeclarations.h"
#include "Octree.h"
#include "UpdatePriorities.h"
#include "Vector.h"
#include "VoxelTraits.h"
#include "WritePermissions.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/RawVolume.h"

#include "PolyVoxCore/LargeVolume.h"

#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

#include "SQLite/sqlite3.h"

namespace Cubiquity
{
	template <typename _VoxelType>
	class Volume
	{
	public:
		typedef _VoxelType VoxelType;

		Volume(const Region& region, const std::string& pathToNewVoxelDatabase, uint32_t baseNodeSize);
		Volume(const std::string& pathToExistingVoxelDatabase, WritePermission writePermission, uint32_t baseNodeSize);
		~Volume();

		// These functions just forward to the underlying PolyVox volume.
		uint32_t getWidth(void) const { return mPolyVoxVolume->getWidth(); }
		uint32_t getHeight(void) const { return mPolyVoxVolume->getHeight(); }
		uint32_t getDepth(void) const { return mPolyVoxVolume->getDepth(); }
		const Region& getEnclosingRegion(void) const { return mPolyVoxVolume->getEnclosingRegion(); }

		// Note this adds a border rather than calling straight through.
		VoxelType getVoxelAt(int32_t x, int32_t y, int32_t z) const;

		// This one's a bit of a hack... direct access to underlying PolyVox volume
		::PolyVox::LargeVolume<VoxelType>* _getPolyVoxVolume(void) const { return mPolyVoxVolume; }

		// Octree access
		Octree<VoxelType>* getOctree(void) { return mOctree; };
		OctreeNode<VoxelType>* getRootOctreeNode(void) { return mOctree->getRootNode(); }

		// Set voxel doesn't just pass straight through, it also validates the position and marks the voxel as modified.
		void setVoxelAt(int32_t x, int32_t y, int32_t z, VoxelType value, UpdatePriority updatePriority = UpdatePriorities::Background);

		// Marks a region as modified so it will be regenerated later.
		void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background);

		void acceptOverrideChunks(void)
		{
			mPolyVoxVolume->flushAll();
			m_pVoxelDatabase->acceptOverrideChunks();
		}
		
		void discardOverrideChunks(void)
		{
			mPolyVoxVolume->flushAll();
			m_pVoxelDatabase->discardOverrideChunks();
		}

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual bool update(const Vector3F& viewPosition, float lodThreshold);

	protected:
		Octree<VoxelType>* mOctree;
		VoxelDatabase<VoxelType>* m_pVoxelDatabase;

	private:
		Volume& operator=(const Volume&);

		::PolyVox::LargeVolume<VoxelType>* mPolyVoxVolume;

		//sqlite3* mDatabase;

		

		// Friend functions
		friend class Octree<VoxelType>;
		friend class OctreeNode<VoxelType>;
	};
}

#include "Volume.inl"

#endif //VOLUME_H_
