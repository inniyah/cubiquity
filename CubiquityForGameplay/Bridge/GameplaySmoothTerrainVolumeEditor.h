#ifndef GAMEPLAY_SMOOTHTERRAINVOLUMEDATA_H_
#define GAMEPLAY_SMOOTHTERRAINVOLUMEDATA_H_

#include "gameplay.h" //Would rather have this last, but it's being upset by Timer.h including Windows.h

#include "GameplaySmoothTerrainVolume.h"
#include "SmoothTerrainVolumeEditor.h"

namespace Cubiquity
{
	/**
	 * Just a dummy comment for now, so that gameplay-luagen will wrap this class.
	 */
	class GameplaySmoothTerrainVolumeEditor
	{
	public:
		GameplaySmoothTerrainVolumeEditor(GameplaySmoothTerrainVolume* volume);
		~GameplaySmoothTerrainVolumeEditor();

		void applyPaint(const gameplay::Vector3& centre, float innerRadius, float outerRadius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount);
		void smooth(const gameplay::Vector3& centre, float innerRadius, float outerRadius, float timeElapsedInSeconds, float amount, float smoothBias);
		void addMaterial(const gameplay::Vector3& centre, float innerRadius, float outerRadius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount);
		void subtractMaterial(const gameplay::Vector3& centre, float innerRadius, float outerRadius, float timeElapsedInSeconds, float amount);

	private:
		SmoothTerrainVolumeImpl* mSmoothTerrainVolume;
	};
}
#endif //GAMEPLAY_SMOOTHTERRAINVOLUMEDATA_H_
