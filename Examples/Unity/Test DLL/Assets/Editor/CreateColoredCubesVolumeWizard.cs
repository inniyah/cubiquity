using UnityEditor;
using UnityEngine;

public class CreateColoredCubesVolumeWizard : ScriptableWizard
{
    private bool createEmptyPressed = true;
	private bool createFromImagesPressed = false;
	private bool createFromHeightmapPressed = false;
    
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
		if(GUILayout.Toggle (createEmptyPressed, "Create empty volume", "button"))
		{
			createEmptyPressed = true;
			createFromImagesPressed = false;
			createFromHeightmapPressed = false;
		}
		
		if(GUILayout.Toggle (createFromImagesPressed, "Create volume from images", "button"))
		{
			createEmptyPressed = false;
			createFromImagesPressed = true;
			createFromHeightmapPressed = false;
		}
		
		if(GUILayout.Toggle (createFromHeightmapPressed, "Create empty volume", "button"))
		{
			createEmptyPressed = false;
			createFromImagesPressed = false;
			createFromHeightmapPressed = true;
		}
	}
}
