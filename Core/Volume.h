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
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"

namespace Cubiquity
{
	// Forward declaration for private volume accessor
	template <typename VoxelType>
	::PolyVox::SimpleVolume<VoxelType>* getPolyVoxVolumeFrom(Volume<VoxelType>* cubiquityVolume);

	template <typename _VoxelType>
	class Volume
	{
	public:
		typedef _VoxelType VoxelType;

		// These functions just forward to the underlying PolyVox volume.
		uint32_t getWidth(void) const { return mPolyVoxVolume->getWidth(); }
		uint32_t getHeight(void) const { return mPolyVoxVolume->getHeight(); }
		uint32_t getDepth(void) const { return mPolyVoxVolume->getDepth(); }
		const Region& getEnclosingRegion(void) const { return mPolyVoxVolume->getEnclosingRegion(); }
		// Note this adds a border rather than calling straight through.
		VoxelType getVoxelAt(int32_t x, int32_t y, int32_t z) { return mPolyVoxVolume->getVoxelWithWrapping(x, y, z, ::PolyVox::WrapModes::Border, VoxelType()); }

		// Octree access
		Octree<VoxelType>* getOctree(void) { return mOctree; };
		OctreeNode<VoxelType>* getRootOctreeNode(void) { return mOctree->getRootNode(); }

		// Set voxel doesn't just pass straight through, it also marks the voxel as modified.
		void setVoxelAt(int32_t x, int32_t y, int32_t z, VoxelType value, UpdatePriority updatePriority = UpdatePriorities::Background);

		// Marks a region as mdified so it will be regenerated later.
		void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background);

		// Should be called before rendering a frame to update the meshes and octree structure.
		virtual void update(const Vector3F& viewPosition, float lodThreshold);

	protected:
		Volume(const Region& region, uint32_t blockSize, OctreeConstructionMode octreeConstructionMode, uint32_t baseNodeSize);
		~Volume();

		Volume& operator=(const Volume&);

	private:
		::PolyVox::SimpleVolume<VoxelType>* mPolyVoxVolume;
		Octree<VoxelType>* mOctree;

		// Friend functions
		friend class Octree<VoxelType>;
		friend class OctreeNode<VoxelType>;
		// This one provides a way for other Cubiquity code to access the PolyVox::Volume without letting user code do it. To enforce this we
		// actually want a nested namespace, but it seems VS2010 has problems with that. See here: http://stackoverflow.com/q/16307836/2337254
		friend ::PolyVox::SimpleVolume<VoxelType>* getPolyVoxVolumeFrom<>(Volume<VoxelType>* cubiquityVolume);
	};

	template <typename VoxelType>
	::PolyVox::SimpleVolume<VoxelType>* getPolyVoxVolumeFrom(Volume<VoxelType>* cubiquityVolume)
	{
		return cubiquityVolume->mPolyVoxVolume;
	}
}

#include "Volume.inl"

#endif //VOLUME_H_
