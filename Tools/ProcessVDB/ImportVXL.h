#ifndef CUBIQUITYTOOLS_IMPORTVXL_H_
#define CUBIQUITYTOOLS_IMPORTVXL_H_

#include "ezOptionParser.hpp"

#include <cstdint>
#include <string>

// Read AoS/B&S map - based on: http://silverspaceship.com/aosmap/aos_file_format.html
bool importVxl(ez::ezOptionParser& options);

#endif //CUBIQUITYTOOLS_IMPORTVXL_H_