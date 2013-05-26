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
	public static extern int cuNewColouredCubesVolume(out uint result, int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, uint blockSize, uint baseNodeSize);
	[DllImport ("CubiquityC")]
	public static extern int cuNewColouredCubesVolumeFromVolDat(out uint result, StringBuilder foldername, uint blockSize, uint baseNodeSize);
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
	public static extern int cuHasRootOctreeNode(out uint result, uint volumeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuGetRootOctreeNode(out uint result, uint volumeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuHasChildNode(out uint result, uint nodeHandle, uint childX, uint childY, uint childZ);
	[DllImport ("CubiquityC")]
	public static extern int cuGetChildNode(out uint result, uint nodeHandle, uint childX, uint childY, uint childZ);
	[DllImport ("CubiquityC")]
	public static extern int cuNodeHasMesh(out uint result, uint nodeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuGetNodePosition(uint nodeHandle, out int x, out int y, out int z);
	[DllImport ("CubiquityC")]
	public static extern int cuGetMeshLastUpdated(out uint result, uint nodeHandle);
	
	// Mesh functions
	[DllImport ("CubiquityC")]
	public static extern int cuGetNoOfVertices(out uint result, uint octreeNodeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuGetNoOfIndices(out uint result, uint octreeNodeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuGetVertices(out IntPtr result, uint octreeNodeHandle);
	[DllImport ("CubiquityC")]
	public static extern int cuGetIndices(out IntPtr result, uint octreeNodeHandle);
	
	// Clock functions
	[DllImport ("CubiquityC")]
	public static extern int cuGetCurrentTime(out uint result);
	
	// Raycasting functions
	[DllImport ("CubiquityC")]
	public static extern int cuPickVoxel(out uint result, uint volumeHandle, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, out int resultX, out int resultY, out int resultZ);
}
