#ifndef GAMEPLAY_SMOOTHTERRAINVOLUMEDATA_H_
#define GAMEPLAY_SMOOTHTERRAINVOLUMEDATA_H_

#include "GameplaySmoothTerrainVolume.h"
#include "SmoothTerrainVolumeEditor.h"

#include "gameplay.h"

/**
 * Just a dummy comment for now, so that gameplay-luagen will wrap this class.
 */
class GameplaySmoothTerrainVolumeEditor : public SmoothTerrainVolumeEditor
{
public:
	GameplaySmoothTerrainVolumeEditor(GameplaySmoothTerrainVolume* volume);
	~GameplaySmoothTerrainVolumeEditor();

	void applyPaint(const gameplay::Vector3& centre, float radius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount);
	void smooth(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount, float smoothBias);
	void addMaterial(const gameplay::Vector3& centre, float radius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount);
	void subtractMaterial(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount);
};

#endif //GAMEPLAY_SMOOTHTERRAINVOLUMEDATA_H_
