#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <complex>
#include <cstdint>
#include <iostream>
#include <sstream>

using namespace std;

const uint32_t Size = 64;

uint32_t evaluateMandlebulbSample(double cx, double cy, double cz, double n, uint32_t maxIterations)
{
	// program from http://www.treblig.org/3dbrot/3dbrot.c
	double x,y,z;
	double newx,newy,newz;
	double theta,phi,rpow;
	double r;
	unsigned int i;
	x=0.0;
	y=0.0;
	z=0.0;

	for(i=0;(i<maxIterations) && ((x*x+y*y+z*z) < 2.0);i++)
	{
		// Maths from http://www.skytopia.com/project/fractal/mandelbulb.html 
		r = sqrt(x*x + y*y + z*z );
		theta = atan2(sqrt(x*x + y*y) , z);
		phi = atan2(y,x);
		rpow = pow(r,n);

		newx = rpow * sin(theta*n) * cos(phi*n);
		newy = rpow * sin(theta*n) * sin(phi*n);
		newz = rpow * cos(theta*n);

		x=newx+cx;
		y=newy+cy;
		z=newz+cz;
	}
	return i;
}

int main(int argc, char** argv)
{
	uint8_t image[Size][Size];

	for(int z = 0; z < Size; z++)
	{
		for(int y = 0; y < Size; y++)
		{
			for(int x = 0; x < Size; x++)
			{
				// Map positions into the range -1.0 to +1.0
				double xPos = x / (static_cast<double>(Size) / 2.0) - 1.0;
				double yPos = y / (static_cast<double>(Size) / 2.0) - 1.0;
				double zPos = z / (static_cast<double>(Size) / 2.0) - 1.0;
				image[x][y] = evaluateMandlebulbSample(xPos, yPos, zPos, 8.0, 255);
			}
		}

		stringstream ss;
		ss << z << ".png";
		int result = stbi_write_png(ss.str().c_str(), Size, Size, 1, image, Size);
		cout << z << endl;
	}

	return 0;
}