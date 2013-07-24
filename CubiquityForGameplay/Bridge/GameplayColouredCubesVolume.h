#ifndef GAMEPLAYCOLOUREDCUBESVOLUME_H_
#define GAMEPLAYCOLOUREDCUBESVOLUME_H_

#include "ColouredCubesVolume.h"

#include "gameplay.h"
#include "Slider.h"

#include "Impl/GameplayVolume.h"

namespace Cubiquity
{

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
		static GameplayColouredCubesVolume* create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, const char* pageFolder, unsigned int baseNodeSize)
		{
			GameplayColouredCubesVolume* volume = new GameplayColouredCubesVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, pageFolder, baseNodeSize);
			return volume;
		}

		/**
		 * Creates a new GameplayColouredCubesVolume.
		 *
		 * @script{create}
		 */
		static GameplayColouredCubesVolume* create(const char* dataToLoad, const char* pageFolder, unsigned int baseNodeSize)
		{
			GameplayColouredCubesVolume* volume = new GameplayColouredCubesVolume(dataToLoad, pageFolder, baseNodeSize);
			return volume;
		}

		// Ugly hack, as luagen can't see the base class implementation of this function (probably it can't handle templated base classes)
		gameplay::Node* getRootNodeForLua(int dummyParamForLuagen)
		{
			return GameplayVolume<ColouredCubesVolume>::getRootNode();
		}

		gameplay::Vector4 getVoxel(int x, int y, int z);
		void setVoxel(int x, int y, int z, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, int updatePriority = 1);
		void markAsModified(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, int updatePriority = 1);

		void performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold);

	protected:
		/**
		 * Text here...
		 * @script{ignore}
		 */
		GameplayColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, const char* pageFolder, unsigned int baseNodeSize);
		/**
		 * Text here...
		 * @script{ignore}
		 */
		GameplayColouredCubesVolume(const char* dataToLoad, const char* pageFolder, unsigned int baseNodeSize);
		virtual ~GameplayColouredCubesVolume();

	private:

		gameplay::Model* buildModelFromPolyVoxMesh(const PolyVox::SurfaceMesh< typename VoxelTraits<typename CubiquityVolumeType::VoxelType>::VertexType>* polyVoxMesh);
	};

}

#endif //GAMEPLAYCOLOUREDCUBESVOLUME_H_