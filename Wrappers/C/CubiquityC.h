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
	CUBIQUITYC_API int32_t cuNewColouredCubesVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, const char* filename, uint32_t baseNodeSize, uint32_t* result);
	CUBIQUITYC_API int32_t cuNewColouredCubesVolumeFromVolDat(const char* volDatToImport, const char* filename, uint32_t baseNodeSize, uint32_t* result);
	CUBIQUITYC_API int32_t cuNewColouredCubesVolumeFromHeightmap(const char* heightmapFileName, const char* colormapFileName, const char* filename, uint32_t baseNodeSize, uint32_t* result);
	CUBIQUITYC_API int32_t cuUpdateVolume(uint32_t volumeHandle);
	CUBIQUITYC_API int32_t cuDeleteColouredCubesVolume(uint32_t volumeHandle);

	CUBIQUITYC_API int32_t cuGetEnclosingRegion(uint32_t volumeHandle, int32_t* lowerX, int32_t* lowerY, int32_t* lowerZ, int32_t* upperX, int32_t* upperY, int32_t* upperZ);
	CUBIQUITYC_API int32_t cuGetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint8_t* red, uint8_t* green, uint8_t* blue, uint8_t* alpha);
	CUBIQUITYC_API int32_t cuSetVoxel(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

	CUBIQUITYC_API int32_t cuAcceptOverrideBlocks(uint32_t volumeHandle);
	CUBIQUITYC_API int32_t cuDiscardOverrideBlocks(uint32_t volumeHandle);

	CUBIQUITYC_API int32_t cuNewSmoothTerrainVolume(int32_t lowerX, int32_t lowerY, int32_t lowerZ, int32_t upperX, int32_t upperY, int32_t upperZ, const char* filename, uint32_t baseNodeSize, uint32_t createFloor, uint32_t floorDepth, uint32_t* result);
	CUBIQUITYC_API int32_t cuUpdateVolumeMC(uint32_t volumeHandle);
	CUBIQUITYC_API int32_t cuDeleteSmoothTerrainVolume(uint32_t volumeHandle);

	CUBIQUITYC_API int32_t cuGetEnclosingRegionMC(uint32_t volumeHandle, int32_t* lowerX, int32_t* lowerY, int32_t* lowerZ, int32_t* upperX, int32_t* upperY, int32_t* upperZ);
	CUBIQUITYC_API int32_t cuGetVoxelMC(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint32_t index, uint8_t* value);
	CUBIQUITYC_API int32_t cuSetVoxelMC(uint32_t volumeHandle, int32_t x, int32_t y, int32_t z, uint32_t index, uint8_t value);

	CUBIQUITYC_API int32_t cuAcceptOverrideBlocksMC(uint32_t volumeHandle);
	CUBIQUITYC_API int32_t cuDiscardOverrideBlocksMC(uint32_t volumeHandle);

	// Octree functions
	CUBIQUITYC_API int32_t cuHasRootOctreeNode(uint32_t volumeHandle, uint32_t* result);
	CUBIQUITYC_API int32_t cuGetRootOctreeNode(uint32_t volumeHandle, uint32_t* result);
	CUBIQUITYC_API int32_t cuHasChildNode(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result);
	CUBIQUITYC_API int32_t cuGetChildNode(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result);
	CUBIQUITYC_API int32_t cuNodeHasMesh( uint32_t nodeHandle, uint32_t* result);
	CUBIQUITYC_API int32_t cuGetNodePosition(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z);
	CUBIQUITYC_API int32_t cuGetMeshLastUpdated(uint32_t nodeHandle, uint32_t* result);

	CUBIQUITYC_API int32_t cuHasRootOctreeNodeMC(uint32_t volumeHandle, uint32_t* result);
	CUBIQUITYC_API int32_t cuGetRootOctreeNodeMC(uint32_t volumeHandle, uint32_t* result);
	CUBIQUITYC_API int32_t cuHasChildNodeMC(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result);
	CUBIQUITYC_API int32_t cuGetChildNodeMC(uint32_t nodeHandle, uint32_t childX, uint32_t childY, uint32_t childZ, uint32_t* result);
	CUBIQUITYC_API int32_t cuNodeHasMeshMC( uint32_t nodeHandle, uint32_t* result);
	CUBIQUITYC_API int32_t cuGetNodePositionMC(uint32_t nodeHandle, int32_t* x, int32_t* y, int32_t* z);
	CUBIQUITYC_API int32_t cuGetMeshLastUpdatedMC(uint32_t nodeHandle, uint32_t* result);

	// Mesh functions
	CUBIQUITYC_API int32_t cuGetNoOfVertices(uint32_t nodeHandle, uint32_t* result);
	CUBIQUITYC_API int32_t cuGetNoOfIndices(uint32_t nodeHandle, uint32_t* result);

	CUBIQUITYC_API int32_t cuGetVertices(uint32_t nodeHandle, float** result);
	CUBIQUITYC_API int32_t cuGetIndices(uint32_t nodeHandle, uint32_t** result);

	CUBIQUITYC_API int32_t cuGetNoOfVerticesMC(uint32_t nodeHandle, uint32_t* result);
	CUBIQUITYC_API int32_t cuGetNoOfIndicesMC(uint32_t nodeHandle, uint32_t* result);

	CUBIQUITYC_API int32_t cuGetVerticesMC(uint32_t nodeHandle, float** result);
	CUBIQUITYC_API int32_t cuGetIndicesMC(uint32_t nodeHandle, uint32_t** result);

	// Clock functions
	CUBIQUITYC_API int32_t cuGetCurrentTime(uint32_t* result);

	// Raycasting functions
	CUBIQUITYC_API int32_t cuPickFirstSolidVoxel(uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ, uint32_t* result);
	CUBIQUITYC_API int32_t cuPickLastEmptyVoxel(uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, int32_t* resultX, int32_t* resultY, int32_t* resultZ, uint32_t* result);

	CUBIQUITYC_API int32_t cuPickTerrainSurface(uint32_t volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, float* resultX, float* resultY, float* resultZ, uint32_t* result);

	// Editing functions
	CUBIQUITYC_API int32_t cuSculptSmoothTerrainVolume(uint32_t volumeHandle, float brushX, float brushY, float brushZ, float brushInnerRadius, float brushOuterRadius, float opacity);
	CUBIQUITYC_API int32_t cuBlurSmoothTerrainVolume(uint32_t volumeHandle, float brushX, float brushY, float brushZ, float brushInnerRadius, float brushOuterRadius, float opacity);
	CUBIQUITYC_API int32_t cuPaintSmoothTerrainVolume(uint32_t volumeHandle, float brushX, float brushY, float brushZ, float brushInnerRadius, float brushOuterRadius, float opacity, uint32_t materialIndex);
}
