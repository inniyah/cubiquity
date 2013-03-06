#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <cstdint>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    uint8_t image[256][256];

	for(int y = 0; y < 256; y++)
	{
		for(int x = 0; x < 256; x++)
		{
			image[x][y] = x;
		}
	}

	int result = stbi_write_png("output.png", 256, 256, 1, image, 256);

	cout << result <<endl;

    return 0;
}