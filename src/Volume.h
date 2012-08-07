#ifndef VOLUME_H_
#define VOLUME_H_

#include "VolumeRegion.h"

#include "Node.h"
#include "Ref.h"

#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"

class Volume : public gameplay::Ref
{
public:	
	static Volume* create();

	gameplay::Node* getRootNode();
	void setVoxelAt(int x, int y, int z, PolyVox::Material8 value);

private:
	Volume();
	~Volume();
	Volume& operator=(const Volume&);

	void loadData();
	void updateMeshes();

public:
	PolyVox::SimpleVolume<PolyVox::Material8>* mVolData;
	gameplay::Node* mRootNode;
	VolumeRegion* mVolumeRegion;
};

#endif //VOLUME_H_
