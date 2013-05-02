// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CUBIQUITYC_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CUBIQUITYC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CUBIQUITYC_EXPORTS
#define CUBIQUITYC_API __declspec(dllexport)
#else
#define CUBIQUITYC_API __declspec(dllimport)
#endif

extern "C"
{
	// Volume functions
	CUBIQUITYC_API unsigned int newColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, unsigned int blockSize, unsigned int baseNodeSize);
	CUBIQUITYC_API void updateVolume(unsigned int volumeHandle);

	// Octree functions
	CUBIQUITYC_API unsigned int getRootOctreeNode(unsigned int volumeHandle);

	// Mesh functions
	CUBIQUITYC_API unsigned int getNoOfVertices(unsigned int volumeHandle, unsigned int octreeNodeHandle);
	CUBIQUITYC_API unsigned int getNoOfIndices(unsigned int volumeHandle, unsigned int octreeNodeHandle);

	CUBIQUITYC_API float* getVertices(unsigned int volumeHandle, unsigned int octreeNodeHandle);
	CUBIQUITYC_API unsigned int* getIndices(unsigned int volumeHandle, unsigned int octreeNodeHandle);
}
