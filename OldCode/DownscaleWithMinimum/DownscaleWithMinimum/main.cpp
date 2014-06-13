#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>

#include "stb_image.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		cout << "ERROR: Not enough parameters" << endl;
	}

	int inputWidth, inputHeight, inputN;
	char* input = argv[1];
	unsigned char *inputData = stbi_load(input, &inputWidth, &inputHeight, &inputN, 0);

	if(inputData == 0)
	{
		cout << stbi_failure_reason() << endl;
	}

	if(inputN > 1)
	{
		cout << "ERROR: Only single channel images are supported" << endl;
	}

	int outputWidth = inputWidth / 2;
	int outputHeight = inputHeight / 2;
	int outputN = inputN;

	int outputDataSize = outputWidth * outputHeight * outputN;
	unsigned char* outputData = new unsigned char[outputDataSize];

	for(int x = 0; x < inputWidth - 1; x++)
	{
		for(int y = 0; y < inputHeight - 1; y++)
		{
			unsigned char in00 = *(inputData + ((y + 0) * inputWidth + (x + 0)) * inputN);
			unsigned char in01 = *(inputData + ((y + 0) * inputWidth + (x + 1)) * inputN);
			unsigned char in10 = *(inputData + ((y + 1) * inputWidth + (x + 0)) * inputN);
			unsigned char in11 = *(inputData + ((y + 1) * inputWidth + (x + 1)) * inputN);

			unsigned char minPixel = min(min(in00, in01), min(in10, in11));

			*(outputData + ((y / 2) * outputWidth + (x / 2)) * outputN) = minPixel;
		}
	}

	int result = stbi_write_png(argv[2], outputWidth, outputHeight, outputN, outputData, outputWidth * outputN);
	assert(result); //If crashing here then make sure the output folder exists.

	delete[] outputData;

	return 0;
}