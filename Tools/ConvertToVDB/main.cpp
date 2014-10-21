#include "main.h"

#include "CmdOption.h"
#include "ImportHeightmap.h"
#include "ImportImageSlices.h"
#include "ImportMagicaVoxel.h"
#include "ImportVXL.h"

#include "CubiquityC.h"

#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	InputFormat inputFormat = InputFormats::Unknown;
	string inputFilenameOrFolder;
	if (cmdOptionExists(argv, argv + argc, "-Heightmap"))
	{
		inputFormat = InputFormats::Heightmap;
		inputFilenameOrFolder = getCmdOption(argv, argv + argc, "-Heightmap");
	}
	else if (cmdOptionExists(argv, argv + argc, "-ImageSlices"))
	{
		inputFormat = InputFormats::ImageSlices;
		inputFilenameOrFolder = getCmdOption(argv, argv + argc, "-ImageSlices");
	}
	else if (cmdOptionExists(argv, argv + argc, "-MagicaVoxel"))
	{
		inputFormat = InputFormats::MagicaVoxel;
		inputFilenameOrFolder = getCmdOption(argv, argv + argc, "-MagicaVoxel");
	}
	else if (cmdOptionExists(argv, argv + argc, "-VXL"))
	{
		inputFormat = InputFormats::VXL;
		inputFilenameOrFolder = getCmdOption(argv, argv + argc, "-VXL");
	}
	else
	{
		cerr << "No input format specified!" << endl << endl;
		printUsage();
		return EXIT_FAILURE;
	}

	uint32_t outputFormat = CU_UNKNOWN;
	string outputFilename;
	if (cmdOptionExists(argv, argv + argc, "-ColoredCubesVolume"))
	{
		outputFormat = CU_COLORED_CUBES;
		outputFilename = getCmdOption(argv, argv + argc, "-ColoredCubesVolume");
	}
	else if (cmdOptionExists(argv, argv + argc, "-TerrainVolume"))
	{
		outputFormat = CU_TERRAIN;
		outputFilename = getCmdOption(argv, argv + argc, "-TerrainVolume");
	}
	else
	{
		cerr << "No input format specified!" << endl << endl;
		printUsage();
		return EXIT_FAILURE;
	}

	// If the output file already exists then we need to delete
	// it before we can use the filename for the new volume.
	remove(outputFilename.c_str());

	switch(inputFormat)
	{
	case InputFormats::Heightmap:
		// User might have provided a colormap as well as the heightmap
		importHeightmap(inputFilenameOrFolder, getCmdOption(argv, argv + argc, "-Colormap"), outputFilename, outputFormat);
		break;
	case InputFormats::ImageSlices:
		importImageSlices(inputFilenameOrFolder, outputFilename, outputFormat);
		break;
	case InputFormats::MagicaVoxel:
		importMagicaVoxel(inputFilenameOrFolder, outputFilename, outputFormat);
		break;
	case InputFormats::VXL:
		importVxl(inputFilenameOrFolder, outputFilename, false, outputFormat);
		break;
	default:
		cerr << "Unrecognised input format" << endl;
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