#ifndef CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_
#define CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_

#include "HeaderOnlyLibs.h"

#include <cstdint>
#include <string>
#include <vector>

bool importHeightmap(ez::ezOptionParser& options);
bool importHeightmapAsColoredCubesVolume(ez::ezOptionParser& options);
bool importHeightmapAsTerrainVolume(ez::ezOptionParser& options);

#endif //CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_
