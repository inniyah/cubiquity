using UnityEngine;
using UnityEditor;
using System.Collections;

[CustomEditor(typeof(ColoredCubesVolume))]
public class ColoredCubesVolumeEditor : Editor
{ 	
	[MenuItem("Cubiqity/Create Empty Colored Cubes Volume")]
    static void CreateEmpty()
    {
		if(GameObject.Find("Voxel Terrain") != null)
		{
			Debug.LogError("A voxel terrain already exists - you (currently) can't create another one.");
			return;
		}
		
		GameObject VoxelTerrainRoot = new GameObject("Voxel Terrain");
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.pageFolder = "D:/temp/voldata";
    }
	
	[MenuItem("Cubiqity/Import Colored Cubes Volume from VolDat")]
    static void ImportFromVolDat()
    {
		if(GameObject.Find("Voxel Terrain") != null)
		{
			Debug.LogError("A voxel terrain already exists - you (currently) can't create another one.");
			return;
		}
		
		GameObject VoxelTerrainRoot = new GameObject("Voxel Terrain");
		VoxelTerrainRoot.AddComponent<ColoredCubesVolume>();
		
		ColoredCubesVolume coloredCubesVolume = VoxelTerrainRoot.GetComponent<ColoredCubesVolume>();
		coloredCubesVolume.folderName = "Assets/Resources/VoxeliensTerrain/";
    }
	
	/*void OnEnable()
	{
		Debug.Log ("ColoredCubesVolumeEditor.OnEnable()");
		
		ColoredCubesVolume coloredCubesVolume = (ColoredCubesVolume)target;
		coloredCubesVolume.Initialize();
		coloredCubesVolume.Synchronize();
	}
	
	void OnDisable()
	{
		Debug.Log ("ColoredCubesVolumeEditor.OnDisable()");
		
		ColoredCubesVolume coloredCubesVolume = (ColoredCubesVolume)target;
		coloredCubesVolume.Shutdown();
	}*/
}