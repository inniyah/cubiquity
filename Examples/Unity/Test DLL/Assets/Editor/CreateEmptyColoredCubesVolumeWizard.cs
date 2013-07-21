using UnityEditor;
using UnityEngine;

using System;
using System.Collections;
using System.IO;

public class CreateEmptyColoredCubesVolumeWizard : ScriptableWizard
{
	private int lowerX = 0;
	private int lowerY = 0;
	private int lowerZ = 0;
	
	private int upperX = 255;
	private int upperY = 255;
	private int upperZ = 255;
	
	private string datasetName = "New Volume";
	
	[MenuItem ("GameObject/Create Other/Colored Cubes Volume/Create Empty Colored Cubes Volume...")]
    static void CreateWizard ()
	{
        ScriptableWizard.DisplayWizard<CreateEmptyColoredCubesVolumeWizard>("Create Empty Colored Cubes Volume");
        //If you don't want to use the secondary button simply leave it out:
        //ScriptableWizard.DisplayWizard<WizardCreateLight>("Create Light", "Create");
    }
	
	void OnGUI()
	{
		EditorGUILayout.LabelField("Some text explaining about the folder...");
		EditorGUILayout.BeginHorizontal();	
			GUILayout.Space(20);
			EditorGUILayout.TextField("Folder name:", datasetName);
			if(GUILayout.Button("..."))
			{
				string selectedFolder = EditorUtility.SaveFolderPanel("Create or choose and empty folder for the volume data", Cubiquity.pathToData, "");
				
				Uri selectedUri = new Uri(selectedFolder);
				Uri assetUrl = new Uri(Application.dataPath);
				Uri executableUri = new Uri(assetUrl, ".");
				Uri relativeUri = executableUri.MakeRelativeUri(selectedUri);
			
				datasetName = Uri.UnescapeDataString(relativeUri.ToString());
			}
			GUILayout.Space(20);
		EditorGUILayout.EndHorizontal();
		
		GUILayout.Space(20);
		
		EditorGUILayout.LabelField("Some text explaining about the dimensions...");
		EditorGUILayout.BeginHorizontal();	
			GUILayout.Space(20);		
			EditorGUILayout.BeginVertical();
				lowerX = EditorGUILayout.IntField("Lower X bound", lowerX);
				lowerY = EditorGUILayout.IntField("Lower Y bound", lowerY);
				lowerZ = EditorGUILayout.IntField("Lower Z bound", lowerZ);
			EditorGUILayout.EndVertical();
		
			EditorGUILayout.BeginVertical();
				upperX = EditorGUILayout.IntField("Upper X bound", upperX);
				upperY = EditorGUILayout.IntField("Upper Y bound", upperY);
				upperZ = EditorGUILayout.IntField("Upper Z bound", upperZ);
			EditorGUILayout.EndVertical();
		EditorGUILayout.EndHorizontal();
		
		GUILayout.Space(50); // A space before the create/cancel buttons
		
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
	}
	
	void OnCancelPressed()
	{
		Debug.Log("Cancelling");
		Close ();
	}
	
	/// <summary>
    /// Creates a relative path from one file or folder to another.
    /// </summary>
    /// <param name="fromPath">Contains the directory that defines the start of the relative path.</param>
    /// <param name="toPath">Contains the path that defines the endpoint of the relative path.</param>
    /// <param name="dontEscape">Boolean indicating whether to add uri safe escapes to the relative path</param>
    /// <returns>The relative path from the start directory to the end path.</returns>
    /// <exception cref="ArgumentNullException"></exception>
    public String MakeRelativePath(String fromPath, String toPath)
    {
        if (String.IsNullOrEmpty(fromPath)) throw new ArgumentNullException("fromPath");
        if (String.IsNullOrEmpty(toPath))   throw new ArgumentNullException("toPath");

        Uri fromUri = new Uri(fromPath);
        Uri toUri = new Uri(toPath);

        Uri relativeUri = fromUri.MakeRelativeUri(toUri);
        String relativePath = Uri.UnescapeDataString(relativeUri.ToString());

        return relativePath.Replace('/', Path.DirectorySeparatorChar);
    }
}
