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
		}
		
		ColoredCubesVolumeFactory.CreateVolume("Voxel Terrain", new Region(0, 0, 0, 16, 16, 16), "D:/temp/voldata", 16);
    }
	
	[MenuItem("Cubiqity/Import Colored Cubes Volume from VolDat")]
    static void ImportFromVolDat()
    {
		if(GameObject.Find("Voxel Terrain") != null)
		{
			Debug.LogError("A voxel terrain already exists - you (currently) can't create another one.");
		}
		
		ColoredCubesVolumeFactory.CreateVolumeFromVolDat("Voxel Terrain", "Assets/Resources/VoxeliensTerrain/", "./", 16);
    }
}