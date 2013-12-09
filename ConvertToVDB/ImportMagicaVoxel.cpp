#include "ImportMagicaVoxel.h"

#include "MagicaVoxelModel.h"

#include "CubiquityC.h"

#include <iostream>

using namespace std;

bool importMagicaVoxel(const std::string& filename, const std::string& pathToVoxelDatabase)
{
	MV_Model model;
	model.LoadModel(filename.c_str());

	uint32_t volumeHandle;
	if(cuNewEmptyColoredCubesVolume(0, 0, 0, model.sizex - 1, model.sizey - 1, model.sizez - 1, pathToVoxelDatabase.c_str(), 32, &volumeHandle) != 1) //FIXME - Hardcoded return value!
	{
		cerr << "Failed to create new empty volume" << endl;
		return false;
	}

	for(int i = 0; i < model.numVoxels; i++)
	{
		MV_RGBA rgba = model.palette[model.voxels[i].colorIndex];

		CuColor color;
		cuBuildColor(rgba.r, rgba.g, rgba.b, rgba.a, &color);

		if(cuSetVoxel(volumeHandle, model.voxels[i].x, model.voxels[i].y, model.voxels[i].z, color) != 1) // FIXME - Hardcoded return value.
		{
			cerr << "Error setting voxel color" << endl;
			return false;
		}
	}

	cuAcceptOverrideBlocks(volumeHandle);
	cuDeleteColoredCubesVolume(volumeHandle);

	return true;
}