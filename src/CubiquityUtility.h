#ifndef CUBIQUITY_UTILITY_H_
#define CUBIQUITY_UTILITY_H_

#include "Colour.h"
#include "MultiMaterial.h"

#include "PolyVoxCore/RawVolume.h"

void rescaleCubicVolume(PolyVox::RawVolume<Colour>* pVolSrc, const PolyVox::Region& regSrc, PolyVox::RawVolume<Colour>* pVolDst, const PolyVox::Region& regDst);

void rescaleCubicVolume(PolyVox::RawVolume<MultiMaterial4>* pVolSrc, const PolyVox::Region& regSrc, PolyVox::RawVolume<MultiMaterial4>* pVolDst, const PolyVox::Region& regDst);

#endif //CUBIQUITY_UTILITY_H_