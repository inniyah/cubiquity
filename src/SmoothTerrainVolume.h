#ifndef SMOOTHTERRAINVOLUME_H_
#define SMOOTHTERRAINVOLUME_H_

#include "MultiMaterial.h"
#include "Volume.h"

#include "PolyVoxCore/MaterialDensityPair.h"

/*void copyVolume(const SimpleVolume<MultiMaterial4>* src, SimpleVolume<MultiMaterial4>* dst)
{
	Region reg = src->getEnclosingRegion();
	int minX = reg.getLowerCorner().getX();
	int minY = reg.getLowerCorner().getY();
	int minZ = reg.getLowerCorner().getZ();
	int maxX = reg.getUpperCorner().getX();
	int maxY = reg.getUpperCorner().getY();
	int maxZ = reg.getUpperCorner().getZ();
	
	for(int z = minZ; z <= maxZ; z++)
	{
		for(int y = minY; y <= maxY; y++)
		{
			for(int x = minX; x <= maxX; x++)
			{
				dst->setVoxelAt(x,y,z,src->getVoxelAt(x,y,z));
			}
		}
	}
}*/

namespace EditActions
{
	enum EditAction
	{
		Add,
		Subtract,
		Paint, 
		Smooth
	};
}
typedef EditActions::EditAction EditAction;

class SmoothTerrainVolume : public Volume<MultiMaterial4>
{
public:
	static SmoothTerrainVolume* create(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	{
		SmoothTerrainVolume* volume = new SmoothTerrainVolume(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);
		return volume;
	}

	void edit(const gameplay::Vector3& centre, float radius, uint32_t materialToUse, EditAction editAction, float timeElapsedInSeconds, float amount, float smoothBias);
	void addToMaterial(uint32_t index, uint8_t amountToAdd, MultiMaterial4& material);
	void subtractFromMaterial(uint8_t amountToAdd, MultiMaterial4& material);

	void applyPaint(const gameplay::Vector3& centre, float radius, uint32_t materialToPaintWith, float timeElapsedInSeconds, float amount, float smoothBias);
	void smooth(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount, float smoothBias);
	void addMaterial(const gameplay::Vector3& centre, float radius, uint32_t materialToAdd, float timeElapsedInSeconds, float amount, float smoothBias);
	void subtractMaterial(const gameplay::Vector3& centre, float radius, float timeElapsedInSeconds, float amount, float smoothBias);

protected:
	SmoothTerrainVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
		:Volume<MultiMaterial4>(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth) {}
};

#endif //SMOOTHTERRAINVOLUME_H_
