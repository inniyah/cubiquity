#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <complex>
#include <cstdint>
#include <iostream>

using namespace std;

 void draw_Mandelbrot(uint8_t image[256][256],                                   //where to draw the image
                      uint8_t set_color, uint8_t non_set_color,       //which colors to use for set/non-set points
                      double cxmin, double cxmax, double cymin, double cymax,//the rect to draw in the complex plane
                      unsigned int max_iterations)                          //the maximum number of iterations
{
  std::size_t const ixsize = 256;
  std::size_t const iysize = 256;
  for (std::size_t ix = 0; ix < ixsize; ++ix)
    for (std::size_t iy = 0; iy < iysize; ++iy)
    {
      std::complex<double> c(cxmin + ix/(ixsize-1.0)*(cxmax-cxmin), cymin + iy/(iysize-1.0)*(cymax-cymin));
      std::complex<double> z = 0;
      unsigned int iterations;
 
      for (iterations = 0; iterations < max_iterations && std::abs(z) < 2.0; ++iterations) 
        z = z*z + c;
 
      image[ix][iy] = (iterations == max_iterations) ? set_color : non_set_color;
 
    }
}

int main(int argc, char** argv)
{
    uint8_t image[256][256];

	draw_Mandelbrot(image, 255, 0, -1, 1, -1, 1, 200);

	/*for(int y = 0; y < 256; y++)
	{
		for(int x = 0; x < 256; x++)
		{
			image[x][y] = x;
		}
	}*/

	int result = stbi_write_png("output.png", 256, 256, 1, image, 256);

	cout << result <<endl;

    return 0;
}