#include "Import.h"

#include "ImportHeightmap.h"
#include "ImportImageSlices.h"
#include "ImportMagicaVoxel.h"
#include "ImportVXL.h"

#include "CmdOption.h"

#include "CubiquityC.h"

#include <iostream>

using namespace ez;
using namespace std;

int import(int argc, const char* argv[], ezOptionParser& options)
{
	string outputFilename;
	
	if (options.isSet("-coloredcubes"))
		options.get("-coloredcubes")->getString(outputFilename);
	if (options.isSet("-terrain"))
		options.get("-terrain")->getString(outputFilename);
	
	// If the output file already exists then we need to delete
	// it before we can use the filename for the new volume.
	remove(outputFilename.c_str());

	if (options.isSet("-heightmap"))
	{
		importHeightmap(options);
	}
	else if (options.isSet("-imageslices"))
	{
		importImageSlices(options);
	}

	return 0;
}

void printUsage(void)
{
	cout << "Usage: ConvertToVDB -InputFormat inputFileOrFolder -OutputFormat outputFile" << endl;
	cout << "" << endl;
	cout << "Where:" << endl;
	cout << "\tInputFormat is from {ImageSlices, MagicaVoxel, VXL}" << endl;
	cout << "\tOutputFormat is from {ColoredCubesVolume, TerrainVolume}" << endl;
	cout << "" << endl;
	cout << "\tE.g. 'ConvertToVDB -VXL my_map.vxl -ColoredCubesVolume my_output.vdb'" << endl;
	cout << "" << endl;
}