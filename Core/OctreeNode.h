#ifndef OCTREE_NODE_H_
#define OCTREE_NODE_H_

#include "Clock.h"
#include "Colour.h"

#include "MultiMaterialMarchingCubesController.h"
#include "MultiMaterial.h"

#include "VoxelTraits.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceMesh.h"

template <typename VoxelType>
class Volume;

template <typename VertexType>
class OctreeNode
{
public:	
	OctreeNode(PolyVox::Region region, OctreeNode* parentRegion);
	~OctreeNode();

	void markDataAsModified(int32_t x, int32_t y, int32_t z, Timestamp newTimeStamp);
	void markDataAsModified(const PolyVox::Region& region, Timestamp newTimeStamp);

	void clearWantedForRendering(void);
	void determineWantedForRendering(const PolyVox::Vector3DFloat& viewPosition, float threshold);
	void determineWhetherToRender(void);

	bool hasAnyChildren(void);

	bool allChildrenUpToDate(void);

	bool isMeshUpToDate(void);
	bool isSceduledForUpdate(void);

	void setMeshLastUpdated(Timestamp newTimeStamp);

	PolyVox::Region mRegion;
	Timestamp mDataLastModified;
	Timestamp mMeshLastUpdated;
	Timestamp mLastSceduledForUpdate;

	OctreeNode* parent;
	OctreeNode* children[2][2][2];

	bool mWantedForRendering;
	bool mRenderThisNode;

	//const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< MultiMaterialMarchingCubesController< MultiMaterial >::MaterialType > >* mSmoothPolyVoxMesh;
	//const PolyVox::SurfaceMesh< PolyVox::PositionMaterial<Colour> >* mCubicPolyVoxMesh;

	const PolyVox::SurfaceMesh<VertexType>* mPolyVoxMesh;

	void* mGameEngineNode;

	uint8_t mLodLevel; // Zero for leaf nodes.
};

#include "OctreeNode.inl"

#endif //OCTREE_NODE_H_
