#include "GameplaySmoothTerrainVolumeEditor.h"

#include "Brush.h"
#include "SmoothTerrainVolumeEditor.h"

namespace Cubiquity
{
	GameplaySmoothTerrainVolumeEditor::GameplaySmoothTerrainVolumeEditor(GameplaySmoothTerrainVolume* volume)
	{
		mSmoothTerrainVolume = dynamic_cast<SmoothTerrainVolumeImpl*>(volume->getCubiquityVolume());
	}

	GameplaySmoothTerrainVolumeEditor::~GameplaySmoothTerrainVolumeEditor()
	{
	}

	void GameplaySmoothTerrainVolumeEditor::applyPaint(const gameplay::Vector3& centre, float innerRadius, float outerRadius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		//edit(v3dCentre, radius, materialToPaintWith, EditActions::Paint, timeElapsedInSeconds,amount, 0.0f);
		Brush brush(innerRadius, outerRadius, amount);
		paintSmoothTerrainVolume(mSmoothTerrainVolume, v3dCentre, brush, materialToPaintWith);
	}

	void GameplaySmoothTerrainVolumeEditor::smooth(const gameplay::Vector3& centre, float innerRadius, float outerRadius, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		Brush brush(innerRadius, outerRadius, amount);
		blurSmoothTerrainVolume(mSmoothTerrainVolume, v3dCentre, brush);
	}

	void GameplaySmoothTerrainVolumeEditor::addMaterial(const gameplay::Vector3& centre, float innerRadius, float outerRadius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		Brush brush(innerRadius, outerRadius, amount);
		sculptSmoothTerrainVolume(mSmoothTerrainVolume, v3dCentre, brush);
	}

	void GameplaySmoothTerrainVolumeEditor::subtractMaterial(const gameplay::Vector3& centre, float innerRadius, float outerRadius, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		Brush brush(innerRadius, outerRadius, amount * -1.0f);
		sculptSmoothTerrainVolume(mSmoothTerrainVolume, v3dCentre, brush);
	}
}
