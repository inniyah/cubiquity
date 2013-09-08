#include "GameplaySmoothTerrainVolumeEditor.h"

#include "SmoothTerrainVolumeEditor.h"

namespace Cubiquity
{
	GameplaySmoothTerrainVolumeEditor::GameplaySmoothTerrainVolumeEditor(GameplaySmoothTerrainVolume* volume)
		:SmoothTerrainVolumeEditor(dynamic_cast<SmoothTerrainVolumeImpl*>(volume->getCubiquityVolume()))
	{
	}

	GameplaySmoothTerrainVolumeEditor::~GameplaySmoothTerrainVolumeEditor()
	{
	}

	void GameplaySmoothTerrainVolumeEditor::applyPaint(const gameplay::Vector3& centre, float radius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		edit(v3dCentre, radius, materialToPaintWith, EditActions::Paint, timeElapsedInSeconds,amount, 0.0f);
	}

	void GameplaySmoothTerrainVolumeEditor::smooth(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float smoothBias, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		// '0' is a dummy as the smooth operations smooths *all* materials
		//edit(v3dCentre, radius, 0, EditActions::Smooth, timeElapsedInSeconds, amount, smoothBias);
		//smoothVolume(v3dCentre, radius);
		blurSmoothTerrainVolume(mSmoothTerrainVolume, v3dCentre, radius, amount);
	}

	void GameplaySmoothTerrainVolumeEditor::addMaterial(const gameplay::Vector3& centre, float radius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		sculptSmoothTerrainVolume(mSmoothTerrainVolume, v3dCentre, radius, amount);
	}

	void GameplaySmoothTerrainVolumeEditor::subtractMaterial(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		sculptSmoothTerrainVolume(mSmoothTerrainVolume, v3dCentre, radius, amount * -1.0f);
	}
}
