#include "GameplayTerrainVolumeEditor.h"

#include "Brush.h"
#include "TerrainVolumeEditor.h"

namespace Cubiquity
{
	GameplayTerrainVolumeEditor::GameplayTerrainVolumeEditor(GameplayTerrainVolume* volume)
	{
		mTerrainVolume = dynamic_cast<TerrainVolumeImpl*>(volume->getCubiquityVolume());
	}

	GameplayTerrainVolumeEditor::~GameplayTerrainVolumeEditor()
	{
	}

	void GameplayTerrainVolumeEditor::applyPaint(const gameplay::Vector3& centre, float innerRadius, float outerRadius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		//edit(v3dCentre, radius, materialToPaintWith, EditActions::Paint, timeElapsedInSeconds,amount, 0.0f);
		Brush brush(innerRadius, outerRadius, amount);
		paintTerrainVolume(mTerrainVolume, v3dCentre, brush, materialToPaintWith);
	}

	void GameplayTerrainVolumeEditor::smooth(const gameplay::Vector3& centre, float innerRadius, float outerRadius, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		Brush brush(innerRadius, outerRadius, amount);
		blurTerrainVolume(mTerrainVolume, v3dCentre, brush);
	}

	void GameplayTerrainVolumeEditor::addMaterial(const gameplay::Vector3& centre, float innerRadius, float outerRadius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		Brush brush(innerRadius, outerRadius, amount);
		sculptTerrainVolume(mTerrainVolume, v3dCentre, brush);
	}

	void GameplayTerrainVolumeEditor::subtractMaterial(const gameplay::Vector3& centre, float innerRadius, float outerRadius, float timeElapsedInSeconds, float amount)
	{
		Vector3F v3dCentre(centre.x, centre.y, centre.z);
		Brush brush(innerRadius, outerRadius, amount * -1.0f);
		sculptTerrainVolume(mTerrainVolume, v3dCentre, brush);
	}
}
