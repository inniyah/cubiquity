#include "Export.h"

#include "ExportImageSlices.h"

#include <iostream>

using namespace ez;
using namespace std;

int exportVDB(ezOptionParser& options)
{
	if (options.isSet("-imageslices"))
	{
		exportImageSlices(options);
	}
	else
	{
		cout << "Unknown export type" << endl;
	}

	return 0;
}