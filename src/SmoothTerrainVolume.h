#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "MultiMaterial.h"
#include "Volume.h"

#include "PolyVoxCore/MaterialDensityPair.h"

class SmoothTerrainVolume : public Volume<MultiMaterial4>
{

public:
	static SmoothTerrainVolume* create(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	{
		SmoothTerrainVolume* volume = new SmoothTerrainVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);
		return volume;
	}

	void applyPaint(const gameplay::Vector3& centre, float radius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount);
	void smooth(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount, float smoothBias);
	void addMaterial(const gameplay::Vector3& centre, float radius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount);
	void subtractMaterial(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount);

protected:
	SmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
		:Volume<MultiMaterial4>(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth)
		,mSmoothingVolume(0)
	{
	}

	virtual ~SmoothTerrainVolume()
	{
		if(mSmoothingVolume)
		{
			delete mSmoothingVolume;
			mSmoothingVolume = 0;
		}
	}

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

	RawVolume<MultiMaterial4>* mSmoothingVolume;
};

#endif //SMOOTHTERRAINVOLUME_H_
