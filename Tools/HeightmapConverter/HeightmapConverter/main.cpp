#include <iostream>

#include "ColouredCubesVolume.h"
#include "VolumeSerialisation.h"

#include "stb_image.c"

using namespace std;

int main(int argc, char** argv)
{
	int imageWidth, imageHeight, n;
	uint32_t imageRange = 256;
	unsigned char *heightData = stbi_load("height.png", &imageWidth, &imageHeight, &n, 0);

	int colourImageWidth, colourImageHeight, colourN;
	unsigned char *colourData = stbi_load("colour.png", &colourImageWidth, &colourImageHeight, &colourN, 0);

	Colour topLayer;
	topLayer.setColour(7, 3, 0, 15);

	Colour bottomLayer;
	bottomLayer.setColour(3, 3, 3, 15);

	ColouredCubesVolume volume(0, 0, 0, imageWidth-1, imageRange-1, imageHeight-1, 32, 32);
	for(uint32_t x = 0; x < imageWidth; x++)
	{
		cout << x << endl;
		for(uint32_t y = 0; y < imageHeight; y++)
		{
			unsigned char heightmapValue = *(heightData + (y * imageWidth + x) * n);
			unsigned char* colourmapValue = colourData + (y * imageWidth + x) * colourN;
			unsigned char red = *colourmapValue;
			unsigned char green = *(colourmapValue + 1);
			unsigned char blue = *(colourmapValue + 2);
			for(uint32_t r = 0; r < imageRange; r++) //Note that we traverse y last.
			{
				Colour voxel;
				if(r <= heightmapValue - 10)
				{
					voxel = bottomLayer;
				}
				else if(r <= heightmapValue - 3)
				{
					voxel = topLayer;
				}
				else if(r <= heightmapValue)
				{
					voxel.setColour(red / 16, green / 16, blue / 16, 15);
				}
				else
				{
					voxel.setColour(0, 0, 0, 0);
				}
				volume.setVoxelAt(x, r, y, voxel); //Note suprising order.
			}
		}
	}

	saveData("output.vol", &volume);

	return 0;
}