#include "ImportHeightmap.h"

#include "CubiquityC.h"

#define STBI_HEADER_FILE_ONLY
#include "stb_image.cpp"

#include <iomanip>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <vector>

using namespace std;

bool importHeightmap(const std::string& heightmapFilename, const std::string& colormapFilename, const std::string& pathToVoxelDatabase)
{
	//cout << "Importing images from '" << folder << "' and into '" << pathToVoxelDatabase << "'";
	return true;
}