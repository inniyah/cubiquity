#ifndef CUBIQUITYTOOLS_HEADERONLYLIBS_H_
#define CUBIQUITYTOOLS_HEADERONLYLIBS_H_

// We make use of several header-only external libraries in this tool, and it seems they
// can be sensitive to the exact include order. We also sometimes need to define certain
// constants exactly once per program. We take tare of this in this HeaderOnlyLibs.h/cpp

#include "ezOptionParser.hpp" // Needs to go before 'easylogging++' od compile errors result.
#include "easylogging++.h"

#endif //CUBIQUITYTOOLS_HEADERONLYLIBS_H_