#ifndef __GameplayIsQuadNeeded_H__
#define __GameplayIsQuadNeeded_H__

#include <cassert>

template<typename VoxelType>
class GameplayIsQuadNeeded
{
public:
	bool operator()(VoxelType back, VoxelType front, float& materialToUse)
	{
		if((back > 0) && (front == 0))
		{
			materialToUse = static_cast<float>(back);
			return true;
		}
		else
		{
			return false;
		}
	}
};

template<>
class GameplayIsQuadNeeded< PolyVox::Material16 >
{
public:
	bool operator()(PolyVox::Material16 back, PolyVox::Material16 front, PolyVox::Material16& materialToUse)
	{
		if((back.getMaterial() > 0) && (front.getMaterial() == 0))
		{
			materialToUse = back.getMaterial();
			return true;
		}
		else
		{
			return false;
		}
	}
};

// We never use the cubic surface extractor with MultiMaterial so this is just a dummy specialisation.
template<>
class GameplayIsQuadNeeded< MultiMaterial >
{
public:
	bool operator()(MultiMaterial back, MultiMaterial front, MultiMaterial& materialToUse)
	{
		assert(false);
		return false;
	}
};

#endif //__GameplayIsQuadNeeded_H__