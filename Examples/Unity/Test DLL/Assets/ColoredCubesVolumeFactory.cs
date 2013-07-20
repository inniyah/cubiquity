using UnityEngine;
using System.Collections;

public class ColoredCubesVolumeFactory
{
	private static uint DefaultBaseNodeSize = 32;
	
	public static GameObject CreateVolume(string name, Region region, string datasetName)
	{
		return CreateVolume (name, region, datasetName, DefaultBaseNodeSize);
	}
	
	public static GameObject CreateVolume(string name, Region region, string datasetName, uint baseNodeSize)
	{		
		// Make sure the page folder exists
		CreatedatasetName(datasetName);
		
		GameObject VoxelTerrainRoot = new GameObject(name);
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.region = region;
		coloredCubesVolume.baseNodeSize = (int)baseNodeSize;
		coloredCubesVolume.datasetName = datasetName;
		
		coloredCubesVolume.Initialize();
		
		return VoxelTerrainRoot;
	}
	
	public static GameObject CreateVolumeFromVolDat(string name, string voldatFolder, string datasetName)
	{
		return CreateVolumeFromVolDat(name, voldatFolder, datasetName, DefaultBaseNodeSize);
	}
	
	public static GameObject CreateVolumeFromVolDat(string name, string voldatFolder, string datasetName, uint baseNodeSize)
	{		
		// Make sure the page folder exists
		CreatedatasetName(datasetName);
		
		GameObject VoxelTerrainRoot = new GameObject(name);
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.voldatFolder = voldatFolder;
		coloredCubesVolume.baseNodeSize = (int)baseNodeSize;
		coloredCubesVolume.datasetName = datasetName;
		
		coloredCubesVolume.Initialize();
		
		return VoxelTerrainRoot;
	}
	
	private static void CreatedatasetName(string datasetName)
	{
		System.IO.Directory.CreateDirectory(datasetName);
		System.IO.Directory.CreateDirectory(datasetName + "/override");
	}
}
