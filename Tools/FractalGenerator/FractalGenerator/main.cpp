#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <complex>
#include <cstdint>
#include <iostream>
#include <sstream>

using namespace std;

const uint32_t ImageSize = 256;

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
	uint8_t image[ImageSize][ImageSize];

	double minX = -sqrt(2.0);
	double maxX = sqrt(2.0);
	double minY = -sqrt(2.0);
	double maxY = sqrt(2.0);
	double minZ = -sqrt(2.0);
	double maxZ = sqrt(2.0);

	double stepX = (maxX - minX) / (ImageSize - 1);
	double stepY = (maxY - minY) / (ImageSize - 1);
	double stepZ = (maxZ - minZ) / (ImageSize - 1);

	for(int z = 0; z < ImageSize; z++)
	{
		for(int y = 0; y < ImageSize; y++)
		{
			for(int x = 0; x < ImageSize; x++)
			{
				double xPos = minX + stepX * x;
				double yPos = minY + stepY * y;
				double zPos = minZ + stepZ * z;
				image[x][y] = evaluateMandlebulbSample(xPos, yPos, zPos, 8.0, 255);
			}
		}

		stringstream ss;
		ss << "output/" << z << ".png";
		int result = stbi_write_png(ss.str().c_str(), ImageSize, ImageSize, 1, image, ImageSize);
		assert(result); //If crashing here then make sur the output folder exists.

		cout << z << endl;
	}

	return 0;
}