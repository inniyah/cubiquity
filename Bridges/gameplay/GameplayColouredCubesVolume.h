#ifndef GAMEPLAYCOLOUREDCUBESVOLUME_H_
#define GAMEPLAYCOLOUREDCUBESVOLUME_H_

#include "ColouredCubesVolume.h"

#include "gameplay.h"

#include "Impl/GameplayVolume.h"

class GameplayColouredCubesVolume : public GameplayVolume<ColouredCubesVolume>
{
public:
	static GameplayColouredCubesVolume* create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	{
		GameplayColouredCubesVolume* volume = new GameplayColouredCubesVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize);
		return volume;
	}

	void performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold);

protected:
	GameplayColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);
	virtual ~GameplayColouredCubesVolume();

private:
	void syncNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode);

	gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterial<Colour> >* polyVoxMesh);
};

#endif //GAMEPLAYCOLOUREDCUBESVOLUME_H_
