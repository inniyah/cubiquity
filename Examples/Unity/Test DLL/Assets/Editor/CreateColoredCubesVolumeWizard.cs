using UnityEditor;
using UnityEngine;

public class CreateColoredCubesVolumeWizard : ScriptableWizard
{
    private bool createEmptyPressed = true;
	private bool createFromImagesPressed = false;
	private bool createFromHeightmapPressed = false;
	
	private int lowerX = 0;
	private int lowerY = 0;
	private int lowerZ = 0;
	
	private int upperX = 255;
	private int upperY = 255;
	private int upperZ = 255;
	
	private string pageFolder = "D:/temp/voldata/";
    
    [MenuItem ("GameObject/Create Other/Create Colored Cubes Volume")]
    static void CreateWizard () {
        ScriptableWizard.DisplayWizard<CreateColoredCubesVolumeWizard>("Create Colored Cubes Volume");
        //If you don't want to use the secondary button simply leave it out:
        //ScriptableWizard.DisplayWizard<WizardCreateLight>("Create Light", "Create");
    }
	
	void OnCreatePressed()
	{
		Debug.Log("Creating volume");
		if(createEmptyPressed)
		{
			CreateEmptyVolume();
		}
	}
	
	void OnCancelPressed()
	{
		Debug.Log("Cancelling");
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
	
	void CreateEmptyVolume()
	{
		if(GameObject.Find("Voxel Terrain") != null)
		{
			Debug.LogError("A voxel terrain already exists - you (currently) can't create another one.");
		}
		
		GameObject voxelGameObject = ColoredCubesVolumeFactory.CreateVolume("Voxel Terrain", new Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ), pageFolder, 16);
		ColoredCubesVolume coloredCubesVolume = voxelGameObject.GetComponent<ColoredCubesVolume>();
		
		// Call Initialize so we can start drawing into the volume right away.
		/*coloredCubesVolume.Initialize();
		const int floorDepth = 4;
		Color32 floorColor = new Color32(192, 192, 192, 255);
		
		for(int z = lowerZ; z <= upperZ; z++)
		{
			for(int y = lowerY; y <= lowerY + floorDepth; y++)
			{
				for(int x = lowerX; x <= upperX; x++)
				{
					coloredCubesVolume.SetVoxel(x, y, z, floorColor);
				}
			}
		}*/
	}
}
