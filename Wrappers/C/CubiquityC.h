#include <stdint.h> //C99 fixed size data types.

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
	CUBIQUITYC_API unsigned int newColouredCubesVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, uint32_t blockSize, uint32_t baseNodeSize);
	CUBIQUITYC_API void updateVolume(uint16_t volumeHandle);

	// Octree functions
	CUBIQUITYC_API uint16_t getRootOctreeNode(uint16_t volumeHandle);

	// Mesh functions
	CUBIQUITYC_API unsigned int getNoOfVertices(uint16_t volumeHandle, uint16_t octreeNodeHandle);
	CUBIQUITYC_API unsigned int getNoOfIndices(uint16_t volumeHandle, uint16_t octreeNodeHandle);

	CUBIQUITYC_API float* getVertices(uint16_t volumeHandle, uint16_t octreeNodeHandle);
	CUBIQUITYC_API unsigned int* getIndices(uint16_t volumeHandle, uint16_t octreeNodeHandle);
}
