#ifndef CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_
#define CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_

#include <cstdint>
#include <string>
#include <vector>

bool importHeightmap(const std::string& heightmapFilename, const std::string& colormapFilename, const std::string& pathToVoxelDatabase, uint32_t outputFormat);
bool importHeightmapAsColoredCubesVolume(const std::string& heightmapFilename, const std::string& colormapFilename, const std::string& pathToVoxelDatabase);

#endif //CUBIQUITYTOOLS_IMPORTHEIGHTMAP_H_
