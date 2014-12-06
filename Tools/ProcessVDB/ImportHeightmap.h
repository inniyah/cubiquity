#ifndef CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_
#define CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_

#include "HeaderOnlyLibs.h"

#include <cstdint>
#include <string>
#include <vector>

void importHeightmap(ez::ezOptionParser& options);
void importHeightmapAsColoredCubesVolume(ez::ezOptionParser& options);
void importHeightmapAsTerrainVolume(ez::ezOptionParser& options);

#endif //CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_
