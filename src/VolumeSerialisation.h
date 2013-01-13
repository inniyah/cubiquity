#ifndef CUBIQUITY_VOLUMESERIALISATION_H_
#define CUBIQUITY_VOLUMESERIALISATION_H_

template <typename VolumeType>
void loadData(const char* filename, VolumeType* volume)
{
	FILE* inputFile = fopen(filename, "rb");
	if(!inputFile)
	{
		GP_ERROR("Failed to open volume file");
	}

	fseek(inputFile, 6, SEEK_SET);

	typename VolumeType::VoxelType value;

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volume->mVolData->getWidth(); z++)
	{
		for (int y = 0; y < volume->mVolData->getHeight(); y++)
		{
			for (int x = 0; x < volume->mVolData->getDepth(); x++)
			{
				//Slow and inefficient reading one voxel at a time!
				size_t elementsRead = fread(&value, sizeof(typename VolumeType::VoxelType), 1, inputFile);

				if(elementsRead != 1)
				{
					GP_ERROR("Failed to read voxel %d, %d, %d", x, y, z);
				}

				//Write the voxel value into the volume
				volume->setVoxelAt(x, y, z, value);

			}
		}
	}

	fclose(inputFile);
}

template <typename VolumeType>
void saveData(const char* filename, VolumeType* volume)
{
	FILE* outputFile = fopen(filename, "wb");
	if(!outputFile)
	{
		GP_ERROR("Failed to open volume file");
	}

	uint16_t width = volume->getWidth();
	uint16_t height = volume->getHeight();
	uint16_t depth = volume->getDepth();
	fwrite(&width, sizeof(uint16_t), 1, outputFile);
	fwrite(&height, sizeof(uint16_t), 1, outputFile);
	fwrite(&depth, sizeof(uint16_t), 1, outputFile);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volume->mVolData->getWidth(); z++)
	{
		for (int y = 0; y < volume->mVolData->getHeight(); y++)
		{
			for (int x = 0; x < volume->mVolData->getDepth(); x++)
			{
				typename VolumeType::VoxelType value = volume->getVoxelAt(x,y,z);
				fwrite(&value, sizeof(typename VolumeType::VoxelType), 1, outputFile);
			}
		}
	}

	fclose(outputFile);
}

#endif //CUBIQUITY_VOLUMESERIALISATION_H_
