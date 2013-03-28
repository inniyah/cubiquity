#ifndef VOLUME_H_
#define VOLUME_H_

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
	template <typename _VoxelType>
	class Volume
	{
		friend class Octree<_VoxelType>;
		friend class OctreeNode<_VoxelType>;

	public:
		typedef _VoxelType VoxelType;

		//Getters just forward to the underlying volume
		uint32_t getWidth(void) const { return mPolyVoxVolume->getWidth(); }
		uint32_t getHeight(void) const { return mPolyVoxVolume->getHeight(); }
		uint32_t getDepth(void) const { return mPolyVoxVolume->getDepth(); }
		const Region& getEnclosingRegion(void) const { return mPolyVoxVolume->getEnclosingRegion(); }

		VoxelType getVoxelAt(int32_t x, int32_t y, int32_t z) { return mPolyVoxVolume->getVoxelAt(x, y, z); }

		::PolyVox::SimpleVolume<VoxelType>* _getPolyVoxVolume() { return mPolyVoxVolume; }

		void setVoxelAt(int32_t x, int32_t y, int32_t z, VoxelType value, UpdatePriority updatePriority = UpdatePriorities::Background);

		void markAsModified(const Region& region, UpdatePriority updatePriority = UpdatePriorities::Background);

		virtual void update(const Vector3F& viewPosition, float lodThreshold);

	protected:
		Volume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, uint32_t blockSize, OctreeConstructionMode octreeConstructionMode, uint32_t baseNodeSize);
		~Volume();

		Volume& operator=(const Volume&);

	private:
		::PolyVox::SimpleVolume<VoxelType>* mPolyVoxVolume;
	public:
		Octree<VoxelType>* mOctree;
	};
}

#include "Volume.inl"

#endif //VOLUME_H_
