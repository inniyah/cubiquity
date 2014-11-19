#ifndef CUBIQUITYTOOLS_IMPORTIMAGESLICES_H_
#define CUBIQUITYTOOLS_IMPORTIMAGESLICES_H_

#include "HeaderOnlyLibs.h"

#include <cstdint>
#include <string>
#include <vector>

std::vector<std::string> findImagesInFolder(std::string folder);
bool importImageSlices(ez::ezOptionParser& options);

#endif // CUBIQUITYTOOLS_IMPORTIMAGESLICES_H_