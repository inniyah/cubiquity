#ifndef GAMEPLAYCOLOUREDCUBESVOLUME_H_
#define GAMEPLAYCOLOUREDCUBESVOLUME_H_

#include "gameplay.h" //Would rather have this last, but it's being upset by Timer.h including Windows.h

#include "ColoredCubesVolume.h"

#include "Slider.h"

#include "Impl/GameplayVolume.h"

namespace Cubiquity
{

	/**
	 * A volume containing colored cubes.
	 */
	class GameplayColoredCubesVolume : public gameplay::Ref
	{
	public:
		/**
		 * Creates a new GameplayColoredCubesVolume.
		 *
		 * @script{create}
		 */
		static GameplayColoredCubesVolume* create(const char* pathToVoxelDatabase, unsigned int baseNodeSize)
		{
			GameplayColoredCubesVolume* volume = new GameplayColoredCubesVolume(pathToVoxelDatabase, baseNodeSize);
			return volume;
		}

		gameplay::Node* getRootNode(void)
		{
			return mRootGameplayOctreeNode->mGameplayNode;
		}

		//Not sure I like exposing this one... should make some functions/classes friends instead?
		ColoredCubesVolume* getCubiquityVolume(void)
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

		gameplay::Vector4 getVoxel(int x, int y, int z);
		void setVoxel(int x, int y, int z, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, int updatePriority = 1);
		void markAsModified(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, int updatePriority = 1);

		void performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold);

		gameplay::PhysicsCollisionShape::Definition buildCollisionObjectFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterial< Color > >* polyVoxMesh);

		void syncNode(OctreeNode< Color >* octreeNode, GameplayOctreeNode< Color >* gameplayOctreeNode);

	protected:
		/**
		 * Text here...
		 * @script{ignore}
		 */
		GameplayColoredCubesVolume(const char* pathToVoxelDatabase, unsigned int baseNodeSize);
		virtual ~GameplayColoredCubesVolume();

	private:

		gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< ::PolyVox::PositionMaterial<Color> >* polyVoxMesh);

		ColoredCubesVolume* mCubiquityVolume;
		GameplayOctreeNode< Color >* mRootGameplayOctreeNode;
	};

}

#endif //GAMEPLAYCOLOUREDCUBESVOLUME_H_
