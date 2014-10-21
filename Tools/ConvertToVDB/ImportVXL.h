#ifndef CUBIQUITYTOOLS_IMPORTVXL_H_
#define CUBIQUITYTOOLS_IMPORTVXL_H_

#include <cstdint>
#include <string>

// Read AoS/B&S map - based on: http://silverspaceship.com/aosmap/aos_file_format.html
bool importVxl(const std::string& vxlFilename, const std::string& pathToVoxelDatabase, bool dryRun, uint32_t outputFormat);

#endif //CUBIQUITYTOOLS_IMPORTVXL_H_