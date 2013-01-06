#ifndef OCTREE_NODE_H_
#define OCTREE_NODE_H_

#include "Node.h"
#include "Ref.h"

#include "Colour.h"

#include "GameplayMarchingCubesController.h"
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
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< GameplayMarchingCubesController< MultiMaterial4 >::MaterialType > >* polyVoxMesh);

	// Although we never use Colour with PositionMaterialNormal or MultiMaterial with PositionMaterial the
	// compiler still needs these to exist because it compiles all combinations. So we just provide dummy functions.
	void buildGraphicsMesh(const PolyVox::SurfaceMesh<PolyVox::PositionMaterial<MultiMaterial4> >* polyVoxMesh);
	void buildGraphicsMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< Colour > >* polyVoxMesh);

	void setMaterial(const char* material); //Should be const material - fix gameplay

	void markDataAsModified(int32_t x, int32_t y, int32_t z, uint32_t newTimeStamp);

	void clearWantedForRendering(void);

	bool hasAnyChildren(void);

	bool allChildrenUpToDate(void);

	bool isMeshUpToDate(void);

	void setMeshLastUpdated(uint32_t newTimeStamp);

	uint32_t depth(void);

	gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< GameplayMarchingCubesController< MultiMaterial4 >::MaterialType > >* polyVoxMesh);
	gameplay::PhysicsCollisionShape::Definition buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< GameplayMarchingCubesController< MultiMaterial4 >::MaterialType > >* polyVoxMesh);

	PolyVox::Region mRegion;
	gameplay::Node* mNode;
	//bool mIsMeshUpToDate;
	uint32_t mMeshLastUpdated;
	uint32_t mDataLastModified;

	OctreeNode* parent;
	OctreeNode* children[2][2][2];

	bool mWantedForRendering;

	const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< GameplayMarchingCubesController< MultiMaterial4 >::MaterialType > >* mPolyVoxMesh;
};

#endif //OCTREE_NODE_H_
