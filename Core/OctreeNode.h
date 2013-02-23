#ifndef OCTREE_NODE_H_
#define OCTREE_NODE_H_

#include "Clock.h"
#include "Colour.h"

#include "MultiMaterialMarchingCubesController.h"
#include "MultiMaterial.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/SurfaceMesh.h"

template <typename VoxelType>
class Volume;

class OctreeNode
{
public:	
	OctreeNode(PolyVox::Region region, OctreeNode* parentRegion);
	~OctreeNode();

	// Builds the graphics mesh for cubic terrain (Colour material type and PositionMaterial vertex type)
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<Colour> >* polyVoxMesh);
	// Builds the graphics mesh for smooth terrain (MultiMaterial material type and PositionMaterialNormal vertex type)
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< MultiMaterialMarchingCubesController< MultiMaterial4 >::MaterialType > >* polyVoxMesh);

	// Although we never use Colour with PositionMaterialNormal or MultiMaterial with PositionMaterial the
	// compiler still needs these to exist because it compiles all combinations. So we just provide dummy functions.
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<MultiMaterial4> >* polyVoxMesh);
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< Colour > >* polyVoxMesh);

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

	const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< MultiMaterialMarchingCubesController< MultiMaterial4 >::MaterialType > >* mSmoothPolyVoxMesh;
	const PolyVox::SurfaceMesh< PolyVox::PositionMaterial<Colour> >* mCubicPolyVoxMesh;

	void* mGameEngineNode;

	uint8_t mLodLevel; // Zero for leaf nodes.
};

#endif //OCTREE_NODE_H_
