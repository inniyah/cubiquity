#include "main.h"

#include "Import.h"
#include "Export.h"

#include "CubiquityC.h"

#include <cstdio>
#include <iostream>
#include <string>

#include "ezOptionParser.hpp"

using namespace ez;
using namespace std;

// Sample command lines
// -import -heightmap C:\temp\temp\height.png -terrain C:\code\cubiquity\Data\exported_volume.vdb -scale 0.25
// -import -imageslices C:\code\cubiquity\Data\ImageSlices\VoxeliensTerrain -coloredcubes C:\code\cubiquity\Data\exported_volume.vdb
// -import -magicavoxel C:\code\cubiquity\Data\MagicaVoxel\scene_store3.vox -coloredcubes C:\code\cubiquity\Data\exported_volume.vdb
// -import -vxl C:\code\cubiquity\Data\VXL\RealisticBridge.vxl -coloredcubes C:\code\cubiquity\Data\exported_volume.vdb
//
// -export -coloredcubes C:\code\cubiquity\Data\exported_volume.vdb -imageslices C:\code\cubiquity\Data\ImageSlices\ExportedVolume

int main(int argc, const char* argv[])
{
	ezOptionParser options;

	options.overview = "Demo of parser's features.";
	options.syntax = "complete first second [OPTIONS] in1 [... inN] out";
	options.example = "complete a b -f --list 1,2,3 --list 4,5,6,7,8 -s string -int -2147483648,2147483647 -ulong 9223372036854775807 -float 3.40282e+038 -double 1.79769e+308 f1 f2 f3 f4 f5 f6 fout\n\n";
	options.footer = "ezOptionParser 0.1.4  Copyright (C) 2011 Remik Ziemlinski\nThis program is free and without warranty.\n";

	// We have to declare here all options which we might later want to check for. Unfortunaltly ezOptionParser does not support 'increamental',
	// parsing which would let us for example reparse the command line looking for a input format once we had establised that we wanted to import.
	// Format for adding an option is:
	// 
	//   options.add(Default, Required?, Number of args expected, Delimiter if expecting multiple args, Help description, Flag token, Flag token);
	
	// Mode of operation
	options.add("", 0, 0, 0, "Import volume data.", "-import", "--import");
	options.add("", 0, 0, 0, "Export volume data.", "-export", "--export");

	// Recognised data formats
	options.add("", 0, 1, 0, "A grayscale image representing a heightmap.", "-heightmap", "--heightmap");
	options.add("", 0, 1, 0, "A color image corresponding to a heightmap.", "-colormap", "--colormap");
	options.add("", 0, 1, 0, "A folder containing a series of images representing slices through the volume.", "-imageslices", "--imageslices");
	options.add("", 0, 1, 0, "The format used by the MagicaVoxel modelling application.", "-magicavoxel", "--magicavoxel");
	options.add("", 0, 1, 0, "The format used by the game 'Build and Shoot', and possibly other games built on the 'Voxlap' engine.", "-vxl", "--vxl");

	// Volume formats
	options.add( "", 0, 1, 0, "A volume consisting of colored cubes.", "-coloredcubes", "--coloredcubes" );
	options.add("", 0, 1, 0, "A volume representing a terrain with each voxel being a 'MaterialSet'.", "-terrain", "--terrain");

	// Other parameters.
	options.add("1.0", 0, 1, 0, "Scale factor" "-scale", "--scale");

	options.parse(argc, argv);

	if (options.isSet("--import"))
	{
		return import(options);
	}
	else if (options.isSet("--export"))
	{
		return export(options);
	}
	else
	{
		std::cout << "Unknown operation" << std::endl;
	}

	return 0;
}