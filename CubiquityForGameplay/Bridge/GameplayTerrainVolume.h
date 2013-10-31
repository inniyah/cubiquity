#ifndef GAMEPLAYTERRAINVOLUME_H_
#define GAMEPLAYTERRAINVOLUME_H_

#include "gameplay.h" //Would rather have this last, but it's being upset by Timer.h including Windows.h

#include "TerrainVolume.h"
#include "SmoothSurfaceExtractionTask.h"

#include "Impl/GameplayVolume.h"

namespace Cubiquity
{
	/**
	 * A volume containing smooth terrain.
	 */
	class GameplayTerrainVolume : public gameplay::Ref
	{
	public:
		/**
		 * Creates a new GameplayTerrainVolume.
		 *
		 * @script{create}
		 */
		static GameplayTerrainVolume* create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, const char* pageFolder, unsigned int baseNodeSize, bool createFloor, unsigned int floorDepth)
		{
			GameplayTerrainVolume* volume = new GameplayTerrainVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, pageFolder, baseNodeSize, createFloor, floorDepth);
			return volume;
		}

		/**
		 * Creates a new GameplayTerrainVolume.
		 *
		 * @script{create}
		 */
		static GameplayTerrainVolume* create(const char* dataToLoad, const char* pageFolder, unsigned int baseNodeSize)
		{
			GameplayTerrainVolume* volume = new GameplayTerrainVolume(dataToLoad, pageFolder, baseNodeSize);
			return volume;
		}

		gameplay::Node* getRootNode(void)
		{
			return mRootGameplayOctreeNode->mGameplayNode;
		}

		//Not sure I like exposing this one... should make some functions/classes friends instead?
		TerrainVolume* getCubiquityVolume(void)
		{
			return mCubiquityVolume;
		}

		uint32_t getWidth(void)
		{
			return mCubiquityVolume->getWidth();
		}

		uint32_t getHeight(void)
		{
			return mCubiquityVolume->getHeight();
		}

		uint32_t getDepth(void)
		{
			return mCubiquityVolume->getDepth();
		}

		// Ugly hack, as luagen can't see the base class implementation of this function (probably it can't handle templated base classes)
		/*gameplay::Node* getRootNodeForLua(int dummyParamForLuagen)
		{
			return mRootGameplayOctreeNode->mGameplayNode;
		}*/

		void performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold);

		gameplay::PhysicsCollisionShape::Definition buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterialNormal< MaterialSet > >* polyVoxMesh);

		void syncNode(OctreeNode< MaterialSet >* octreeNode, GameplayOctreeNode< MaterialSet >* gameplayOctreeNode);

	protected:
		GameplayTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, const char* pageFolder, unsigned int baseNodeSize, bool createFloor, unsigned int floorDepth);
		GameplayTerrainVolume(const char* dataToLoad, const char* pageFolder, unsigned int baseNodeSize);
		virtual ~GameplayTerrainVolume();

	private:

		gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterialNormal< MaterialSet > >* polyVoxMesh);

		TerrainVolume* mCubiquityVolume;
		GameplayOctreeNode< MaterialSet >* mRootGameplayOctreeNode;
	};
}

#endif //GAMEPLAYTERRAINVOLUME_H_
