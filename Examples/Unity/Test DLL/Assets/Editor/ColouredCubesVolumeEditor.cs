using UnityEngine;
using UnityEditor;
using System.Collections;

[CustomEditor(typeof(ColouredCubesVolume))]
public class ColouredCubesVolumeEditor : Editor
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
		VoxelTerrainRoot.AddComponent<ColouredCubesVolume>();
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
		VoxelTerrainRoot.AddComponent<ColouredCubesVolume>();
		
		ColouredCubesVolume colouredCubesVolume = VoxelTerrainRoot.GetComponent<ColouredCubesVolume>();
		colouredCubesVolume.folderName = "C:/Code/cubiquity/Examples/SliceData/VoxeliensTerrain/";
    }
	
	/*void OnEnable()
	{
		Debug.Log ("ColouredCubesVolumeEditor.OnEnable()");
		
		ColouredCubesVolume colouredCubesVolume = (ColouredCubesVolume)target;
		colouredCubesVolume.Initialize();
		colouredCubesVolume.Synchronize();
	}
	
	void OnDisable()
	{
		Debug.Log ("ColouredCubesVolumeEditor.OnDisable()");
		
		ColouredCubesVolume colouredCubesVolume = (ColouredCubesVolume)target;
		colouredCubesVolume.Shutdown();
	}*/
}