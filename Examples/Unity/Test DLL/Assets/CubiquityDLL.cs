using System;
using System.Runtime.InteropServices;
using System.Text;

public class CubiquityDLL
{
	public static void Validate(int returnCode)
	{
		if(returnCode < 0)
		{
			throw new CubiquityException("An exception occured inside Cubiquity. Please see the log file for details");
		}
	}
	
	// Volume functions
	[DllImport ("CubiquityC")]
	public static extern int cuNewColouredCubesVolume(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, uint blockSize, uint baseNodeSize, out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuNewColouredCubesVolumeFromVolDat(StringBuilder foldername, uint blockSize, uint baseNodeSize, out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuUpdateVolume(uint volumeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuGetVoxel(uint volumeHandle, int x, int y, int z, out byte red, out byte green, out byte blue, out byte alpha);
	[DllImport ("CubiquityC")]
	public static extern int cuSetVoxel(uint volumeHandle, int x, int y, int z, byte red, byte green, byte blue, byte alpha);
	[DllImport ("CubiquityC")]
	public static extern int cuDeleteColouredCubesVolume(uint volumeHandle);
	
	// Octree functions
	[DllImport ("CubiquityC")]
	public static extern int cuHasRootOctreeNode(uint volumeHandle, out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuGetRootOctreeNode(uint volumeHandle,out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuHasChildNode(uint nodeHandle, uint childX, uint childY, uint childZ, out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuGetChildNode(uint nodeHandle, uint childX, uint childY, uint childZ, out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuNodeHasMesh(uint nodeHandle, out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuGetNodePosition(uint nodeHandle, out int x, out int y, out int z);
	[DllImport ("CubiquityC")]
	public static extern int cuGetMeshLastUpdated(uint nodeHandle, out uint result);
	
	// Mesh functions
	[DllImport ("CubiquityC")]
	public static extern int cuGetNoOfVertices(uint octreeNodeHandle, out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuGetNoOfIndices(uint octreeNodeHandle, out uint result);
	[DllImport ("CubiquityC")]
	public static extern int cuGetVertices(uint octreeNodeHandle, out IntPtr result);
	[DllImport ("CubiquityC")]
	public static extern int cuGetIndices(uint octreeNodeHandle, out IntPtr result);
	
	// Clock functions
	[DllImport ("CubiquityC")]
	public static extern int cuGetCurrentTime(out uint result);
	
	// Raycasting functions
	[DllImport ("CubiquityC")]
	public static extern int cuPickVoxel(uint volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, out int resultX, out int resultY, out int resultZ, out uint result);
}
