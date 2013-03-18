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
		static GameplayColouredCubesVolume* create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize)
		{
			GameplayColouredCubesVolume* volume = new GameplayColouredCubesVolume(lowerX, lowerY, lowerZ, upperX, upperY, upperZ, blockSize, baseNodeSize);
			return volume;
		}

		/**
		 * Creates a new GameplayColouredCubesVolume.
		 *
		 * @script{create}
		 */
		static GameplayColouredCubesVolume* create(const char* volDatFolder)
		{
			GameplayColouredCubesVolume* volume = new GameplayColouredCubesVolume(volDatFolder);
			return volume;
		}

		/**
		 * I don't like exposing the internal cubiquity volume in this public interface.
		 *
		 * @script{create}
		 */
		static GameplayColouredCubesVolume* create(ColouredCubesVolume* cubiquityVolume)
		{
			GameplayColouredCubesVolume* volume = new GameplayColouredCubesVolume(cubiquityVolume);
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
			return GameplayVolume<ColouredCubesVolume>::getCubiquityVolume();
		}

		gameplay::Vector4 getVoxel(int x, int y, int z);
		void setVoxel(int x, int y, int z, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, int updatePriority = 1);
		void markAsModified(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, int updatePriority = 1);

		void performUpdate(const gameplay::Vector3& viewPosition, float lodThreshold);

	protected:
		GameplayColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);
		GameplayColouredCubesVolume(const char* volDatFolder);
		GameplayColouredCubesVolume(ColouredCubesVolume* colouredCubesVolume);
		virtual ~GameplayColouredCubesVolume();

	private:
		void syncNode(OctreeNode< Colour >* octreeNode, gameplay::Node* gameplayNode);

		gameplay::Model* buildModelFromPolyVoxMesh(const ::PolyVox::SurfaceMesh< ::PolyVox::PositionMaterial<Colour> >* polyVoxMesh);
	};

}

#endif //GAMEPLAYCOLOUREDCUBESVOLUME_H_
