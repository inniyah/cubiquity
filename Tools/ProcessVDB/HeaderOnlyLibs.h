#ifndef CUBIQUITYTOOLS_HEADERONLYLIBS_H_
#define CUBIQUITYTOOLS_HEADERONLYLIBS_H_

// Note: Including this file is pretty bad for compile tiimes!
// Maybe consider splittig it up if that becaomes a problem.

// We make use of several header-only external libraries in this tool, and it seems they
// can be sensitive to the exact include order. We also sometimes need to define certain
// constants exactly once per program. We take tare of this in this HeaderOnlyLibs.h/cpp

#include "Dependencies/ezOptionParser.hpp" // Needs to go before 'easylogging++' od compile errors result.
#include "Dependencies/easylogging++.h"

#include "Dependencies/stb_image.h"
#include "Dependencies/stb_image_resize.h"
#include "Dependencies/stb_image_write.h"

#endif //CUBIQUITYTOOLS_HEADERONLYLIBS_H_