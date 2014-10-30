#ifndef CUBIQUITYTOOLS_IMPORTIMAGESLICES_H_
#define CUBIQUITYTOOLS_IMPORTIMAGESLICES_H_

#include <cstdint>
#include <string>
#include <vector>

std::vector<std::string> findImagesInFolder(std::string folder);
bool importImageSlices(const std::string& folder, const std::string& pathToVoxelDatabase, uint32_t outputFormat);

#endif // CUBIQUITYTOOLS_IMPORTIMAGESLICES_H_