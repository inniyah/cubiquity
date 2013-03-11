#include <algorithm>
#include <cstdint>
#include <iostream>
#include <sstream>

#include "stb_image.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

int main(int argc, char** argv)
{
	int imageWidth, imageHeight, n;
	uint32_t imageRange = 256;
	uint32_t outputChannels = 4;
	unsigned char *heightData = stbi_load("height.png", &imageWidth, &imageHeight, &n, 0);

	int colourImageWidth, colourImageHeight, colourN;
	unsigned char *colourData = stbi_load("colour.png", &colourImageWidth, &colourImageHeight, &colourN, 0);

	int outputSliceDataSize = imageWidth * imageHeight * outputChannels;
	unsigned char* outputSliceData = new unsigned char[outputSliceDataSize];

	for(uint32_t z = 0; z < imageRange; z++)
	{
		cout << z << endl;

		std::fill(outputSliceData, outputSliceData + imageWidth * imageHeight, 0);

		bool anyPixelsSet = false;

		for(uint32_t x = 0; x < imageWidth; x++)
		{
			for(uint32_t y = 0; y < imageHeight; y++)
			{
				int heightmapValue = *(heightData + (y * imageWidth + x) * n);
				unsigned char* colourmapValue = colourData + (y * imageWidth + x) * colourN;
				unsigned char* outputSliceDataValue = outputSliceData + (y * imageWidth + x) * outputChannels;
				unsigned char red = *colourmapValue;
				unsigned char green = *(colourmapValue + 1);
				unsigned char blue = *(colourmapValue + 2);

				unsigned char outputRed = 0, outputGreen = 0, outputBlue = 0, outputAlpha = 0;

				if(static_cast<int>(z) <= heightmapValue - 5)
				{
					outputRed = 127;
					outputGreen = 127;
					outputBlue = 127;
					outputAlpha = 255;
					anyPixelsSet = true;
				}
				else if(z <= heightmapValue)
				{
					outputRed = red;
					outputGreen = green;
					outputBlue = blue;
					outputAlpha = 255;
					anyPixelsSet = true;
				}

				if(anyPixelsSet)
				{
					*outputSliceDataValue = outputRed;
					*(outputSliceDataValue+1) = outputGreen;
					*(outputSliceDataValue+2) = outputBlue;
					*(outputSliceDataValue+3) = outputAlpha;
				}
			}
		}

		stringstream ss;
		ss << "C:/temp/output/" << z << ".png";
		int result = stbi_write_png(ss.str().c_str(), imageWidth, imageHeight, outputChannels, outputSliceData, imageWidth * outputChannels);
		assert(result); //If crashing here then make sure the output folder exists.

		// We're processing a heightmap, so one we have a completely
		// empty slice we know there is no more data higher up
		if(!anyPixelsSet)
		{
			break;
		}
	}

	delete[] outputSliceData;

	return 0;
}