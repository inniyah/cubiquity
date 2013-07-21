using UnityEngine;
using System.Collections;

public static class Cubiquity
{
	// This is the relative path to where the volumes are stored on disk.
	public const string pathToData = "Cubiquity/Volumes/";
	
	public static bool PickVoxel(ColoredCubesVolume volume, float rayStartX, float rayStartY, float rayStartZ, float rayDirX, float rayDirY, float rayDirZ, out int resultX, out int resultY, out int resultZ)
	{
		uint hit = CubiquityDLL.PickVoxel((uint)volume.volumeHandle, rayStartX, rayStartY, rayStartZ, rayDirX, rayDirY, rayDirZ, out resultX, out resultY, out resultZ);
		
		return hit == 1;
	}
}
