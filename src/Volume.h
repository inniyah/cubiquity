#ifndef VOLUME_H_
#define VOLUME_H_

#include "VolumeRegion.h"

#include "Node.h"
#include "Ref.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

class Volume : public gameplay::Ref
{
public:	
	static Volume* create(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth);

	gameplay::Node* getRootNode();

	void setMaterial(const char* materialPath);
	void setVoxelAt(int x, int y, int z, PolyVox::Material16 value);

	void loadData(const char* filename);
	void updateMeshes();

private:
	Volume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth);
	~Volume();
	Volume& operator=(const Volume&);

public:
	PolyVox::SimpleVolume<PolyVox::Material16>* mVolData;
	gameplay::Node* mRootNode;
	//VolumeRegion* mVolumeRegion;
	PolyVox::Array<3, VolumeRegion*> mVolumeRegions;
};

#endif //VOLUME_H_
