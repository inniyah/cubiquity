using UnityEditor;
using UnityEngine;

public class CreateColoredCubesVolumeWizard : ScriptableWizard
{
    private bool createEmptyPressed = true;
	private bool createFromImagesPressed = false;
	private bool createFromHeightmapPressed = false;
	
	private int lowerX = 255;
	private int lowerY = 255;
	private int lowerZ = 255;
	
	private int upperX = 255;
	private int upperY = 255;
	private int upperZ = 255;
	
	private string pageFolder = "Enter path here...";
    
    [MenuItem ("GameObject/Create Other/Create Colored Cubes Volume/Empty Volume")]
    static void CreateWizard () {
        ScriptableWizard.DisplayWizard<CreateColoredCubesVolumeWizard>("Create Empty Colored Cubes Volume", "Create", "Cancel");
        //If you don't want to use the secondary button simply leave it out:
        //ScriptableWizard.DisplayWizard<WizardCreateLight>("Create Light", "Create");
    }
	
    void OnWizardCreate ()
	{
    }  
	
    void OnWizardUpdate ()
	{
    }   
    
    void OnWizardOtherButton ()
	{
        Close ();
    }
	
	void OnGUI()
	{
		EditorGUILayout.BeginHorizontal();	
			GUILayout.Space(120);
			EditorGUILayout.TextField("Save location:", pageFolder);
			if(GUILayout.Button("..."))
			{
				pageFolder = EditorUtility.SaveFolderPanel("Choose save location for volume data", "C:/", "");
			}
		EditorGUILayout.EndHorizontal();
		
		EditorGUILayout.BeginHorizontal();	
			GUILayout.Space(20);
		
			if(GUILayout.Toggle (createEmptyPressed, "Create empty\nvolume", "button", GUILayout.Width (100), GUILayout.Height (100)))
			{
				createEmptyPressed = true;
				createFromImagesPressed = false;
				createFromHeightmapPressed = false;
			}
		
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
		
		EditorGUILayout.BeginHorizontal();
			GUILayout.Space(20);
			if(GUILayout.Toggle (createFromImagesPressed, "Create volume\nfrom images", "button", GUILayout.Width (100), GUILayout.Height (100)))
			{
				createEmptyPressed = false;
				createFromImagesPressed = true;
				createFromHeightmapPressed = false;
			}
		EditorGUILayout.EndHorizontal();
		
		EditorGUILayout.BeginHorizontal();
			GUILayout.Space(20);
			if(GUILayout.Toggle (createFromHeightmapPressed, "Create volume\nfrom heightmap", "button", GUILayout.Width (100), GUILayout.Height (100)))
			{
				createEmptyPressed = false;
				createFromImagesPressed = false;
				createFromHeightmapPressed = true;
			}
		EditorGUILayout.EndHorizontal();
	}
}
