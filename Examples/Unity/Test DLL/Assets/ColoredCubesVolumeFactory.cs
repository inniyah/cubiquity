using UnityEngine;
using System.Collections;

public class ColoredCubesVolumeFactory
{
	public static GameObject CreateVolume(Region region, uint baseNodeSize, string pageFolder)
	{
		if(GameObject.Find("Voxel Terrain") != null)
		{
			Debug.LogError("A voxel terrain already exists - you (currently) can't create another one.");
			return null;
		}
		
		GameObject VoxelTerrainRoot = new GameObject("Voxel Terrain");
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.region = region;
		coloredCubesVolume.baseNodeSize = baseNodeSize;
		coloredCubesVolume.pageFolder = pageFolder;
		
		return VoxelTerrainRoot;
	}
	
	public static GameObject CreateVolumeFromVolDat(string voldatFolder, uint baseNodeSize, string pageFolder)
	{
		if(GameObject.Find("Voxel Terrain") != null)
		{
			Debug.LogError("A voxel terrain already exists - you (currently) can't create another one.");
			return null;
		}
		
		GameObject VoxelTerrainRoot = new GameObject("Voxel Terrain");
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.voldatFolder = voldatFolder;
		coloredCubesVolume.baseNodeSize = baseNodeSize;
		coloredCubesVolume.pageFolder = pageFolder;
		
		return VoxelTerrainRoot;
	}
}
