#ifndef GAMEPLAYVOLUME_H_
#define GAMEPLAYVOLUME_H_

#include "gameplay.h"

#include "Volume.h"

template <typename _VolumeType>
class GameplayVolume : public gameplay::Ref
{
protected:
	GameplayVolume(VolumeType type, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int regionWidth, unsigned int regionHeight, unsigned int regionDepth)
	{
		mVolume = new _VolumeType(type, lowerX, lowerY, lowerZ, upperX, upperY, upperZ, regionWidth, regionHeight, regionDepth);
	}

	~GameplayVolume()
	{
	}

public:
	_VolumeType* mVolume;
};

#endif //GAMEPLAYVOLUME_H_