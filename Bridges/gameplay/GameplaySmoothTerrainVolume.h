#ifndef GAMEPLAYSMOOTHTERRAINVOLUME_H_
#define GAMEPLAYSMOOTHTERRAINVOLUME_H_

#include "SmoothTerrainVolume.h"

#include "gameplay.h"

#include "Impl/GameplayVolume.h"

/**
 * A volume containing smooth terrain.
 */
class GameplaySmoothTerrainVolume : public GameplayVolume<SmoothTerrainVolume>
{
public:
	/**
     * Creates a new GameplaySmoothTerrainVolume.
	 *
     * @script{create}
     */
	static GameplaySmoothTerrainVolume* create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	{
		GameplaySmoothTerrainVolume* volume = new GameplaySmoothTerrainVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize);
		return volume;
	}

	void performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold);

protected:
	GameplaySmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);
	virtual ~GameplaySmoothTerrainVolume();

private:
	void syncNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode);

	gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< MultiMaterialMarchingCubesController< MultiMaterial4 >::MaterialType > >* polyVoxMesh);
	gameplay::PhysicsCollisionShape::Definition buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterialNormal< MultiMaterialMarchingCubesController< MultiMaterial4 >::MaterialType > >* polyVoxMesh);
};

#endif //GAMEPLAYSMOOTHTERRAINVOLUME_H_
