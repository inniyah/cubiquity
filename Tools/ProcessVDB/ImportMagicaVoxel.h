#ifndef CUBIQUITYTOOLS_IMPORTMAGICAVOXEL_H_
#define CUBIQUITYTOOLS_IMPORTMAGICAVOXEL_H_

#include "ezOptionParser.hpp"

#include <cstdint>
#include <string>

// Check is this is a Magica Voxel file.
bool isMagicaVoxel(const std::string& filename);
// Read MagicaVoxel map: https://voxel.codeplex.com/
bool importMagicaVoxel(ez::ezOptionParser& options);

#endif //CUBIQUITYTOOLS_IMPORTMAGICAVOXEL_H_