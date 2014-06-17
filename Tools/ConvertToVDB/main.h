#ifndef CUBIQUITYTOOLS_MAIN_H_
#define CUBIQUITYTOOLS_MAIN_H_

#include <string>

namespace InputFormats
{
	enum InputFormat
	{
		ImageSlices,
		VXL,
		MagicaVoxel,
		Unknown
	};
}
typedef InputFormats::InputFormat InputFormat;

void printUsage(void);
InputFormat determineInputFormat(const std::string& input);

#endif //CUBIQUITYTOOLS_MAIN_H_