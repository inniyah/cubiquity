#include "main.h"

#include "CmdOption.h"
#include "ImportImageSlices.h"
#include "ImportMagicaVoxel.h"
#include "ImportVXL.h"

#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	if(!cmdOptionExists(argv, argv+argc, "-i"))
	{
		cerr << "No input specified!" << endl << endl;
		printUsage();
		return EXIT_FAILURE;
	}

	if(!cmdOptionExists(argv, argv+argc, "-o"))
	{
		cerr << "No output filename specified!" << endl << endl;
		printUsage();
		return EXIT_FAILURE;
	}

	string input = getCmdOption(argv, argv+argc, "-i");
	string outputFilename = getCmdOption(argv, argv+argc, "-o");

	InputFormat inputFormat = determineInputFormat(input);

	// If the output file already exists then we need to delete
	// it before we can use the filename for the new volume.
	remove(outputFilename.c_str());

	switch(inputFormat)
	{
	case InputFormats::ImageSlices:
		importImageSlices(input, outputFilename);
		break;
	case InputFormats::VXL:
		importVxl(input, outputFilename);
		break;
	default:
		cerr << "Unrecognised input format" << endl;
	}

	return 0;
}

void printUsage(void)
{
	cout << "Usage: ConvertToVDB -i inputFileOrFolder -o outputfile" << endl;
}

InputFormat determineInputFormat(const std::string& input)
{
	// Test whether the input represents a folder of images
	std::vector<std::string> imageFiles = findImagesInFolder(input);
	if(imageFiles.size() > 0)
	{
		return InputFormats::ImageSlices;
	}

	// Determine if we have a VXL file by running the VXL importer in dry-run mode
	if(importVxl(input, "ThisFileShouldNeverExist.vdb", true))
	{
		return InputFormats::VXL;
	}

	return InputFormats::Unknown;
}