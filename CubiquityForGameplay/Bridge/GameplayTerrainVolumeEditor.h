#ifndef GAMEPLAY_TERRAINVOLUMEDATA_H_
#define GAMEPLAY_TERRAINVOLUMEDATA_H_

#include "gameplay.h" //Would rather have this last, but it's being upset by Timer.h including Windows.h

#include "GameplayTerrainVolume.h"
#include "TerrainVolumeEditor.h"

namespace Cubiquity
{
	/**
	 * Just a dummy comment for now, so that gameplay-luagen will wrap this class.
	 */
	class GameplayTerrainVolumeEditor
	{
	public:
		GameplayTerrainVolumeEditor(GameplayTerrainVolume* volume);
		~GameplayTerrainVolumeEditor();

		void applyPaint(const gameplay::Vector3& centre, float innerRadius, float outerRadius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount);
		void smooth(const gameplay::Vector3& centre, float innerRadius, float outerRadius, float timeElapsedInSeconds, float amount);
		void addMaterial(const gameplay::Vector3& centre, float innerRadius, float outerRadius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount);
		void subtractMaterial(const gameplay::Vector3& centre, float innerRadius, float outerRadius, float timeElapsedInSeconds, float amount);

	private:
		TerrainVolume* mTerrainVolume;
	};
}
#endif //GAMEPLAY_TERRAINVOLUMEDATA_H_
