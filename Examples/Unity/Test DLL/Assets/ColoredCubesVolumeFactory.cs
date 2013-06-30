using UnityEngine;
using System.Collections;

public class ColoredCubesVolumeFactory
{
	public static GameObject CreateVolume(string name, Region region, string pageFolder, uint baseNodeSize)
	{		
		GameObject VoxelTerrainRoot = new GameObject(name);
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.region = region;
		coloredCubesVolume.baseNodeSize = baseNodeSize;
		coloredCubesVolume.pageFolder = pageFolder;
		
		return VoxelTerrainRoot;
	}
	
	public static GameObject CreateVolumeFromVolDat(string name, string voldatFolder, string pageFolder, uint baseNodeSize)
	{		
		GameObject VoxelTerrainRoot = new GameObject(name);
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.voldatFolder = voldatFolder;
		coloredCubesVolume.baseNodeSize = baseNodeSize;
		coloredCubesVolume.pageFolder = pageFolder;
		
		return VoxelTerrainRoot;
	}
}
