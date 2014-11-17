#include "main.h"

#include "CmdOption.h"
#include "Import.h"

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

int main(int argc, const char* argv[])
{
	ezOptionParser options;

	options.overview = "Demo of parser's features.";
	options.syntax = "complete first second [OPTIONS] in1 [... inN] out";
	options.example = "complete a b -f --list 1,2,3 --list 4,5,6,7,8 -s string -int -2147483648,2147483647 -ulong 9223372036854775807 -float 3.40282e+038 -double 1.79769e+308 f1 f2 f3 f4 f5 f6 fout\n\n";
	options.footer = "ezOptionParser 0.1.4  Copyright (C) 2011 Remik Ziemlinski\nThis program is free and without warranty.\n";

	options.add(
		"", // Default.
		0, // Required?
		0, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Import volume data.", // Help description.
		"-import",     // Flag token.
		"--import"     // Flag token.
		);

	options.add(
		"", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Import volume data from heightmap.", // Help description.
		"-heightmap",     // Flag token.
		"--heightmap"     // Flag token.
		);

	options.add(
		"", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Import volume data from colormap.", // Help description.
		"-colormap",     // Flag token.
		"--colormap"     // Flag token.
		);

	options.add(
		"", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Import volume data from image slices.", // Help description.
		"-imageslices",     // Flag token.
		"--imageslices"     // Flag token.
		);

	options.add(
		"", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Export volume as colored cubes", // Help description.
		"-coloredcubes",     // Flag token.
		"--coloredcubes"     // Flag token.
		);

	options.add(
		"", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Export volume as terrain", // Help description.
		"-terrain",     // Flag token.
		"--terrain"     // Flag token.
		);

	options.add(
		"1.0", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Scale factor", // Help description.
		"-scale",     // Flag token.
		"--scale"     // Flag token.
		);

	options.add(
		"", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Import volume data from Magica Voxel file.", // Help description.
		"-magicavoxel",     // Flag token.
		"--magicavoxel"     // Flag token.
		);

	options.add(
		"", // Default.
		0, // Required?
		1, // Number of args expected.
		0, // Delimiter if expecting multiple args.
		"Import volume data from Magica Voxel file.", // Help description.
		"-vxl",     // Flag token.
		"--vxl"     // Flag token.
		);

	options.parse(argc, argv);

	if (options.isSet("--import"))
	{
		return import(argc, argv, options);
	}

	return 0;
}