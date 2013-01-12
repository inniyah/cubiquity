#ifndef SMOOTHTERRAINVOLUMEEDITOR_H_
#define SMOOTHTERRAINVOLUMEEDITOR_H_

#include "SmoothTerrainVolume.h"

class SmoothTerrainVolumeEditor
{
public:
	SmoothTerrainVolumeEditor(SmoothTerrainVolume* volume);
	~SmoothTerrainVolumeEditor();

	SmoothTerrainVolume* mVolume;
	

	void applyPaint(const gameplay::Vector3& centre, float radius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount);
	void smooth(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount, float smoothBias);
	void addMaterial(const gameplay::Vector3& centre, float radius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount);
	void subtractMaterial(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount);

private:
	class EditActions
	{
	public:
		enum EditAction
		{
			Add,
			Subtract,
			Paint, 
			Smooth
		};
	};
	typedef EditActions::EditAction EditAction;

	void addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial4& material);
	void subtractFromMaterial(uint8_t amountToAdd, MultiMaterial4& material);
	void edit(const gameplay::Vector3& centre, float radius, uint32_t materialToUse, EditAction editAction, float timeElapsedInSeconds, float amount, float smoothBias);

	PolyVox::RawVolume<MultiMaterial4>* mSmoothingVolume;
};

#endif //SMOOTHTERRAINVOLUMEEDITOR_H_
