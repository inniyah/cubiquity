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
		Volume(const std::string& pathToExistingVoxelDatabase, uint32_t baseNodeSize);
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

		void acceptOverrideBlocks(void)
		{
			mPolyVoxVolume->flushAll();
			m_pVoxelDatabase->acceptOverrideBlocks();
		}
		
		void discardOverrideBlocks(void)
		{
			mPolyVoxVolume->flushAll();
			m_pVoxelDatabase->discardOverrideBlocks();
		}

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold);

	protected:
		int32_t getPropertyAsInt(const std::string& name, int32_t defaultValue);
		float getPropertyAsFloat(const std::string& name, float defaultValue);
		std::string getPropertyAsString(const std::string& name, const std::string& defaultValue);

		void setProperty(const std::string& name, int value);
		void setProperty(const std::string& name, float value);
		void setProperty(const std::string& name, const std::string& value);

		Octree<VoxelType>* mOctree;

	private:
		Volume& operator=(const Volume&);

		bool getProperty(const std::string& name, std::string& value);

		::PolyVox::LargeVolume<VoxelType>* mPolyVoxVolume;

		//::PolyVox::MinizBlockCompressor<VoxelType>* m_pCompressor;
		VoxelDatabase<VoxelType>* m_pVoxelDatabase;

		sqlite3* mDatabase;

		sqlite3_stmt* mSelectPropertyStatement;		
		sqlite3_stmt* mInsertOrReplacePropertyStatement;

		// Friend functions
		friend class Octree<VoxelType>;
		friend class OctreeNode<VoxelType>;
	};
}

#include "Volume.inl"

#endif //VOLUME_H_
