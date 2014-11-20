#include "Import.h"

#include "Exceptions.h"
#include "ImportHeightmap.h"
#include "ImportImageSlices.h"
#include "ImportMagicaVoxel.h"
#include "ImportVXL.h"

#include "CubiquityC.h"

#include <iostream>

using namespace ez;
using namespace std;

void importVDB(ezOptionParser& options)
{
	LOG(INFO) << "Importing voxel database...";

	// Check the filename here, so we can ensure it doesn't already exist.
	string outputFilename;	
	if (options.isSet("-coloredcubes"))
		options.get("-coloredcubes")->getString(outputFilename);
	if (options.isSet("-terrain"))
		options.get("-terrain")->getString(outputFilename);
	
	// If the output file already exists then we need to delete
	// it before we can use the filename for the new volume.
	if (remove(outputFilename.c_str()) == 0)
	{
		LOG(INFO) << "Deleted previous file called \"" << outputFilename << "\"";
	}

	if (options.isSet("-heightmap"))
	{
		importHeightmap(options);
	}
	else if (options.isSet("-imageslices"))
	{
		importImageSlices(options);
	}
	else if (options.isSet("-magicavoxel"))
	{
		importMagicaVoxel(options);
	}
	else if (options.isSet("-vxl"))
	{
		importVxl(options);
	}
	else
	{
		throwException(OptionsError("No valid import format specified."));
	}
}
