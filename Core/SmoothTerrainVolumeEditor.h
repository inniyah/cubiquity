#ifndef SMOOTHTERRAINVOLUMEEDITOR_H_
#define SMOOTHTERRAINVOLUMEEDITOR_H_

#include "CubiquityForwardDeclarations.h"
#include "Vector.h"

namespace Cubiquity
{
	class SmoothTerrainVolumeEditor
	{
	public:
		SmoothTerrainVolumeEditor(SmoothTerrainVolume* volume);
		~SmoothTerrainVolumeEditor();

		SmoothTerrainVolume* mSmoothTerrainVolume;

	protected:
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

		void addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial& material);
		void subtractFromMaterial(uint8_t amountToAdd, MultiMaterial& material);
		void edit(const Vector3F& centre, float radius, uint32_t materialToUse, EditAction editAction, float timeElapsedInSeconds, float amount, float smoothBias);

		void smoothVolume(const Vector3F& centre, float radius);

		::PolyVox::RawVolume<MultiMaterial>* mSmoothingVolume;
	};
}

#endif //SMOOTHTERRAINVOLUMEEDITOR_H_
