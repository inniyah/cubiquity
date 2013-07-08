using UnityEngine;
using System.Collections;

public class ColoredCubesVolumeFactory
{
	private static uint DefaultBaseNodeSize = 32;
	
	public static GameObject CreateVolume(string name, Region region, string pageFolder)
	{
		return CreateVolume (name, region, pageFolder, DefaultBaseNodeSize);
	}
	
	public static GameObject CreateVolume(string name, Region region, string pageFolder, uint baseNodeSize)
	{		
		// Make sure the page folder exists
		CreatePageFolder(pageFolder);
		
		GameObject VoxelTerrainRoot = new GameObject(name);
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.region = region;
		coloredCubesVolume.baseNodeSize = (int)baseNodeSize;
		coloredCubesVolume.pageFolder = pageFolder;
		
		coloredCubesVolume.Initialize();
		
		return VoxelTerrainRoot;
	}
	
	public static GameObject CreateVolumeFromVolDat(string name, string voldatFolder, string pageFolder)
	{
		return CreateVolumeFromVolDat(name, voldatFolder, pageFolder, DefaultBaseNodeSize);
	}
	
	public static GameObject CreateVolumeFromVolDat(string name, string voldatFolder, string pageFolder, uint baseNodeSize)
	{		
		// Make sure the page folder exists
		CreatePageFolder(pageFolder);
		
		GameObject VoxelTerrainRoot = new GameObject(name);
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.voldatFolder = voldatFolder;
		coloredCubesVolume.baseNodeSize = (int)baseNodeSize;
		coloredCubesVolume.pageFolder = pageFolder;
		
		coloredCubesVolume.Initialize();
		
		return VoxelTerrainRoot;
	}
	
	private static void CreatePageFolder(string pageFolder)
	{
		System.IO.Directory.CreateDirectory(pageFolder);
		System.IO.Directory.CreateDirectory(pageFolder + "/override");
	}
}
