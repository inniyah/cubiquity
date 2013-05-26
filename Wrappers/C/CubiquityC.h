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
	CUBIQUITYC_API int32_t cuNewColouredCubesVolume(uint32_t* result, int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, uint32_t blockSize, uint32_t baseNodeSize);
	CUBIQUITYC_API int32_t cuNewColouredCubesVolumeFromVolDat(uint32_t* result, const char* volDatToImport, uint32_t blockSize, uint32_t baseNodeSize);
	CUBIQUITYC_API int32_t cuUpdateVolume(uint32_t volumeHandle);
	CUBIQUITYC_API int32_t cuDeleteColouredCubesVolume(uint32_t volumeHandle);

	CUBIQUITYC_API int32_t cuGetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t* alpha);
	CUBIQUITYC_API int32_t cuSetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

	// Octree functions
	CUBIQUITYC_API int32_t cuHasRootOctreeNode(uint32_t* result, uint32_t volumeHandle);
	CUBIQUITYC_API int32_t cuGetRootOctreeNode(uint32_t* result, uint32_t volumeHandle);
	CUBIQUITYC_API int32_t cuHasChildNode(uint32_t* result, uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ);
	CUBIQUITYC_API int32_t cuGetChildNode(uint32_t* result, uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ);
	CUBIQUITYC_API int32_t cuNodeHasMesh(uint32_t* result, uint32_t nodeHandle);
	CUBIQUITYC_API int32_t cuGetNodePosition(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z);
	CUBIQUITYC_API int32_t cuGetMeshLastUpdated(uint32_t* result, uint32_t nodeHandle);

	// Mesh functions
	CUBIQUITYC_API int32_t cuGetNoOfVertices(uint32_t* result, uint32_t nodeHandle);
	CUBIQUITYC_API int32_t cuGetNoOfIndices(uint32_t* result, uint32_t nodeHandle);

	CUBIQUITYC_API int32_t cuGetVertices(float** result, uint32_t nodeHandle);
	CUBIQUITYC_API int32_t cuGetIndices(uint32_t** result, uint32_t nodeHandle);

	// Clock functions
	CUBIQUITYC_API int32_t cuGetCurrentTime(uint32_t* result);

	// Raycasting functions
	CUBIQUITYC_API int32_t cuPickVoxel(uint32_t* result, uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ);
}
