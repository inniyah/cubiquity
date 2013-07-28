using UnityEditor;
using UnityEngine;

using System;
using System.Collections;
using System.IO;

public class CreateColoredCubesVolumeFromHeightmapWizard : ScriptableWizard
{
	[MenuItem ("GameObject/Create Other/Colored Cubes Volume/Create Colored Cubes Volume From Heightmap...")]
    static void CreateWizard ()
	{
        ScriptableWizard.DisplayWizard<CreateColoredCubesVolumeFromHeightmapWizard>("Create Colored Cubes Volume From Heightmap");
    }
	
	void OnGUI()
	{
		EditorGUILayout.BeginHorizontal();
			EditorGUILayout.Space();
			if(GUILayout.Button("Create volume", GUILayout.Width(128)))
			{
				OnCreatePressed ();
			}
			GUILayout.Space(50);
			if(GUILayout.Button("Cancel", GUILayout.Width(128)))
			{
				OnCancelPressed ();
			}
			EditorGUILayout.Space();
		EditorGUILayout.EndHorizontal();
	}
	
	void OnCreatePressed()
	{
		Close();		
		Debug.Log("Creating volume");
		
		if(GameObject.Find("Voxel Terrain") != null)
		{
			Debug.LogError("A voxel terrain already exists - you (currently) can't create another one.");
		}
		
		GameObject voxelGameObject = ColoredCubesVolumeFactory.CreateVolumeFromHeightmap("Voxel Terrain", "D:/temp/height.png", "D:/temp/color.png", "test");
		//ColoredCubesVolume coloredCubesVolume = voxelGameObject.GetComponent<ColoredCubesVolume>();
	}
	
	void OnCancelPressed()
	{
		Debug.Log("Cancelling");
		Close ();
	}
}