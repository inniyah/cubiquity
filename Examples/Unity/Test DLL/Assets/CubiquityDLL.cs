using System;
using System.Runtime.InteropServices;
using System.Text;

public class CubiquityDLL
{
	// Volume functions
	[DllImport ("CubiquityC")]
	public static extern void cuNewColouredCubesVolume(out uint result, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, uint blockSize, uint baseNodeSize);
	[DllImport ("CubiquityC")]
	public static extern void cuNewColouredCubesVolumeFromVolDat(out uint result, StringBuilder foldername, uint blockSize, uint baseNodeSize);
	[DllImport ("CubiquityC")]
	public static extern void cuUpdateVolume(uint volumeHandle);
	[DllImport ("CubiquityC")]
	public static extern void cuGetVoxel(uint volumeHandle, int x, int y, int z, out byte red, out byte green, out byte blue, out byte alpha);
	[DllImport ("CubiquityC")]
	public static extern void cuSetVoxel(uint volumeHandle, int x, int y, int z, byte red, byte green, byte blue, byte alpha);
	[DllImport ("CubiquityC")]
	public static extern void cuDeleteColouredCubesVolume(uint volumeHandle);
	
	// Octree functions
	[DllImport ("CubiquityC")]
	public static extern void cuHasRootOctreeNode(out uint result, uint volumeHandle);
	[DllImport ("CubiquityC")]
	public static extern void cuGetRootOctreeNode(out uint result, uint volumeHandle);
	[DllImport ("CubiquityC")]
	public static extern void cuHasChildNode(out uint result, uint nodeHandle, uint childX, uint childY, uint childZ);
	[DllImport ("CubiquityC")]
	public static extern void cuGetChildNode(out uint result, uint nodeHandle, uint childX, uint childY, uint childZ);
	[DllImport ("CubiquityC")]
	public static extern int cuNodeHasMesh(uint nodeHandle);
	[DllImport ("CubiquityC")]
	public static extern void cuGetNodePosition(uint nodeHandle, out int x, out int y, out int z);
	[DllImport ("CubiquityC")]
	public static extern int cuGetNodePositionX(uint nodeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuGetNodePositionY(uint nodeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuGetNodePositionZ(uint nodeHandle);
	[DllImport ("CubiquityC")]
	public static extern uint cuGetMeshLastUpdated(uint nodeHandle);
	
	// Mesh functions
	[DllImport ("CubiquityC")]
	public static extern uint cuGetNoOfVertices(uint octreeNodeHandle);
	[DllImport ("CubiquityC")]
	public static extern uint cuGetNoOfIndices(uint octreeNodeHandle);
	[DllImport ("CubiquityC")]
	public static extern IntPtr cuGetVertices(uint octreeNodeHandle);
	[DllImport ("CubiquityC")]
	public static extern IntPtr cuGetIndices(uint octreeNodeHandle);
	
	// Clock functions
	[DllImport ("CubiquityC")]
	public static extern uint cuGetCurrentTime();
	
	// Raycasting functions
	[DllImport ("CubiquityC")]
	public static extern int cuPickVoxel(uint volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, out int resultX, out int resultY, out int resultZ);
}
