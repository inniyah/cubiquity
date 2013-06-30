using UnityEngine;
using UnityEditor;
using System.Collections;

[CustomEditor(typeof(ColoredCubesVolume))]
public class ColoredCubesVolumeEditor : Editor
{ 	
	[MenuItem("Cubiqity/Create Empty Colored Cubes Volume")]
    static void CreateEmpty()
    {
		ColoredCubesVolumeFactory.CreateVolume(new Region(0, 0, 0, 16, 16, 16), "D:/temp/voldata", 16);
    }
	
	[MenuItem("Cubiqity/Import Colored Cubes Volume from VolDat")]
    static void ImportFromVolDat()
    {
		ColoredCubesVolumeFactory.CreateVolumeFromVolDat("Assets/Resources/VoxeliensTerrain/", "./", 16);
    }
}