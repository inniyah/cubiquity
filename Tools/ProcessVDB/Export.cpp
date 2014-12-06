#include "Export.h"

#include "Exceptions.h"
#include "ExportImageSlices.h"

#include <iostream>

using namespace ez;
using namespace std;

void exportVDB(ezOptionParser& options)
{
	LOG(INFO) << "Exporting voxel database...";

	if (options.isSet("-imageslices"))
	{
		exportImageSlices(options);
	}
	else
	{
		throwException(OptionsError("No valid export format specified."));
	}
}