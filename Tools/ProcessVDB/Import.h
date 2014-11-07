#ifndef CUBIQUITYTOOLS_IMPORT_H_
#define CUBIQUITYTOOLS_IMPORT_H_

#include <string>

#include "ezOptionParser.hpp"

namespace InputFormats
{
	enum InputFormat
	{
		Heightmap,
		ImageSlices,
		MagicaVoxel,
		VXL,
		Unknown
	};
}
typedef InputFormats::InputFormat InputFormat;

int import(int argc, const char* argv[], ez::ezOptionParser& options);
InputFormat determineInputFormat(const std::string& input);
void printUsage(void);

#endif //CUBIQUITYTOOLS_IMPORT_H_