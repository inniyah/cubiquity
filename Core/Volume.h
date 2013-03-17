#ifndef VOLUME_H_
#define VOLUME_H_

#include "Octree.h"
#include "UpdatePriorities.h"
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
	public:
		typedef _VoxelType VoxelType;

		Volume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, OctreeConstructionMode octreeConstructionMode, unsigned int baseNodeSize);
		~Volume();

		uint32_t getWidth(void);
		uint32_t getHeight(void);
		uint32_t getDepth(void);

		VoxelType getVoxelAt(int x, int y, int z);
		void setVoxelAt(int x, int y, int z, VoxelType value, UpdatePriority updatePriority = UpdatePriorities::Background);
		void markAsModified(const ::PolyVox::Region& region, UpdatePriority updatePriority = UpdatePriorities::Background);

		virtual void update(const ::PolyVox::Vector3DFloat& viewPosition, float lodThreshold);
	protected:
		Volume& operator=(const Volume&);

	public:
		::PolyVox::SimpleVolume<VoxelType>* mPolyVoxVolume;
		//OctreeNode< VoxelType >* mRootOctreeNode;
		Octree<VoxelType>* mOctree;
	};
}

#include "Volume.inl"

#endif //VOLUME_H_
