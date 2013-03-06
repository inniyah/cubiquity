#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <complex>
#include <cstdint>
#include <iostream>

using namespace std;

unsigned int doPoint(double cx, double cy, double cz)
{
	const unsigned int maxiterations=255;
	const double mandpow=8.0;

	// program from http://www.treblig.org/3dbrot/3dbrot.c
	double x,y,z;
	double newx,newy,newz;
	double theta,phi,rpow;
	double r;
	unsigned int i;
	x=0.0;
	y=0.0;
	z=0.0;

	for(i=0;(i<maxiterations) && ((x*x+y*y+z*z) < 2.0);i++)
	{
		/* These maths from http://www.skytopia.com/project/fractal/mandelbulb.html */
		r = sqrt(x*x + y*y + z*z );
		theta = atan2(sqrt(x*x + y*y) , z);
		phi = atan2(y,x);
		rpow = pow(r,mandpow);

		newx = rpow * sin(theta*mandpow) * cos(phi*mandpow);
		newy = rpow * sin(theta*mandpow) * sin(phi*mandpow);
		newz = rpow * cos(theta*mandpow);

		x=newx+cx;
		y=newy+cy;
		z=newz+cz;
	}
	return i;
}

int main(int argc, char** argv)
{
	uint8_t image[256][256];

	for(int y = 0; y < 256; y++)
	{
		for(int x = 0; x < 256; x++)
		{
			double xPos = x / 256.0;
			double yPos = y / 256.0;
			double zPos = 0.0;
			image[x][y] = doPoint(xPos, yPos, zPos);
		}
	}

	int result = stbi_write_png("output.png", 256, 256, 1, image, 256);

	cout << result <<endl;

	return 0;
}