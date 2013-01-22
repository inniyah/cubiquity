#include "GameplaySmoothTerrainVolumeEditor.h"

GameplaySmoothTerrainVolumeEditor::GameplaySmoothTerrainVolumeEditor(GameplaySmoothTerrainVolume* volume)
	:SmoothTerrainVolumeEditor(volume->mVolume)
{
}

GameplaySmoothTerrainVolumeEditor::~GameplaySmoothTerrainVolumeEditor()
{
}

void GameplaySmoothTerrainVolumeEditor::applyPaint(const gameplay::Vector3& centre, float radius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount)
{
	PolyVox::Vector3DFloat v3dCentre(centre.x, centre.y, centre.z);
	edit(v3dCentre, radius, materialToPaintWith, EditActions::Paint, timeElapsedInSeconds,amount, 0.0f);
}

void GameplaySmoothTerrainVolumeEditor::smooth(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount, float smoothBias)
{
	PolyVox::Vector3DFloat v3dCentre(centre.x, centre.y, centre.z);
	// '0' is a dummy as the smooth operations smooths *all* materials
	edit(v3dCentre, radius, 0, EditActions::Smooth, timeElapsedInSeconds, amount, smoothBias);
}

void GameplaySmoothTerrainVolumeEditor::addMaterial(const gameplay::Vector3& centre, float radius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount)
{
	PolyVox::Vector3DFloat v3dCentre(centre.x, centre.y, centre.z);
	edit(v3dCentre, radius, materialToAdd, EditActions::Add, timeElapsedInSeconds, amount, 0.0f);
}

void GameplaySmoothTerrainVolumeEditor::subtractMaterial(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount)
{
	PolyVox::Vector3DFloat v3dCentre(centre.x, centre.y, centre.z);
	// '0' is a dummy as the subtract operations reduces *all* materials
	edit(v3dCentre, radius, 0, EditActions::Subtract, timeElapsedInSeconds, amount, 0.0f);
}