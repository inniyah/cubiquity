#ifndef GAMEPLAYCOLOUREDCUBESVOLUME_H_
#define GAMEPLAYCOLOUREDCUBESVOLUME_H_

#include "ColouredCubesVolume.h"

#include "gameplay.h"

#include "Impl/GameplayVolume.h"

/**
 * A volume containing coloured cubes.
 */
class GameplayColouredCubesVolume : public GameplayVolume<ColouredCubesVolume>
{
public:
	/**
     * Creates a new GameplayColouredCubesVolume.
	 *
     * @script{create}
     */
	static GameplayColouredCubesVolume* create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
	{
		GameplayColouredCubesVolume* volume = new GameplayColouredCubesVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize);
		return volume;
	}

	// Ugly hack, as luagen can't see the base class implementation of this function (probably it can't handle templated base classes)
	gameplay::Node* getRootNodeForLua(int dummyParamForLuagen)
	{
		return GameplayVolume<ColouredCubesVolume>::getRootNode();
	}

	// Ugly hack, as luagen can't see the base class implementation of this function (probably it can't handle templated base classes)
	//Not sure I like exposing this one... should make some functions/classes friends instead?
	ColouredCubesVolume* getVolumeForLua(int dummyParamForLuagen)
	{
		return GameplayVolume<ColouredCubesVolume>::getVolume();
	}

	gameplay::Vector4 getVoxel(int x, int y, int z);
	void setVoxel(int x, int y, int z, const gameplay::Vector4& colour, bool markAsModified = true);
	void markRegionAsModified(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ);

	void performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold);

protected:
	GameplayColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);
	virtual ~GameplayColouredCubesVolume();

private:
	void syncNode(OctreeNode* octreeNode, gameplay::Node* gameplayNode);

	gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< PolyVox::PositionMaterial<Colour> >* polyVoxMesh);
};

#endif //GAMEPLAYCOLOUREDCUBESVOLUME_H_
