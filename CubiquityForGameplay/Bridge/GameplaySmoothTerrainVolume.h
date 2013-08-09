#ifndef GAMEPLAYSMOOTHTERRAINVOLUME_H_
#define GAMEPLAYSMOOTHTERRAINVOLUME_H_

#include "gameplay.h" //Would rather have this last, but it's being upset by Timer.h including Windows.h

#include "SmoothTerrainVolume.h"
#include "SmoothSurfaceExtractionTask.h"

#include "Impl/GameplayVolume.h"

namespace Cubiquity
{
	/**
	 * A volume containing smooth terrain.
	 */
	class GameplaySmoothTerrainVolume : public gameplay::Ref
	{
	public:
		/**
		 * Creates a new GameplaySmoothTerrainVolume.
		 *
		 * @script{create}
		 */
		static GameplaySmoothTerrainVolume* create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, const char* pageFolder, unsigned int baseNodeSize)
		{
			GameplaySmoothTerrainVolume* volume = new GameplaySmoothTerrainVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, pageFolder, baseNodeSize);
			return volume;
		}

		/**
		 * Creates a new GameplaySmoothTerrainVolume.
		 *
		 * @script{create}
		 */
		static GameplaySmoothTerrainVolume* create(const char* dataToLoad, const char* pageFolder, unsigned int baseNodeSize)
		{
			GameplaySmoothTerrainVolume* volume = new GameplaySmoothTerrainVolume(dataToLoad, pageFolder, baseNodeSize);
			return volume;
		}

		gameplay::Node* getRootNode(void)
		{
			return mRootGameplayOctreeNode->mGameplayNode;
		}

		//Not sure I like exposing this one... should make some functions/classes friends instead?
		SmoothTerrainVolume* getCubiquityVolume(void)
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

		gameplay::PhysicsCollisionShape::Definition buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterialNormal< MultiMaterial > >* polyVoxMesh);

		void syncNode(OctreeNode< MultiMaterial >* octreeNode, GameplayOctreeNode< MultiMaterial >* gameplayOctreeNode);

	protected:
		GameplaySmoothTerrainVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, const char* pageFolder, unsigned int baseNodeSize);
		GameplaySmoothTerrainVolume(const char* dataToLoad, const char* pageFolder, unsigned int baseNodeSize);
		virtual ~GameplaySmoothTerrainVolume();

	private:

		gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterialNormal< MultiMaterial > >* polyVoxMesh);

		SmoothTerrainVolume* mCubiquityVolume;
		GameplayOctreeNode< MultiMaterial >* mRootGameplayOctreeNode;
	};
}

#endif //GAMEPLAYSMOOTHTERRAINVOLUME_H_
