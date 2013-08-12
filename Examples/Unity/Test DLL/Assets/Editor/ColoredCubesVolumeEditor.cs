using UnityEngine;
using UnityEditor;
using System.Collections;
 
[CustomEditor (typeof(ColoredCubesVolume))]
public class ColoredCubesVolumeEditor : Editor
{
	ColoredCubesVolume coloredCubesVolume;
	
	private bool addMode = true;
	private bool deleteMode = false;
	private bool paintMode = false;
	
	Color paintColor = Color.white;
	
	public void OnEnable()
	{
	    coloredCubesVolume = target as ColoredCubesVolume;
	    SceneView.onSceneGUIDelegate = ColoredCubesVolumeUpdate;
	}
	
	public override void OnInspectorGUI()
	{		
		if(EditorGUILayout.Toggle("Add cubes", addMode))
		{
			addMode = true;
			deleteMode = false;
			paintMode = false;
		}
		
		if(EditorGUILayout.Toggle("Delete cubes", deleteMode))
		{
			addMode = false;
			deleteMode = true;
			paintMode = false;
		}
		
		if(EditorGUILayout.Toggle("Paint cubes", paintMode))
		{
			addMode = false;
			deleteMode = false;
			paintMode = true;
		}
		
		paintColor = EditorGUILayout.ColorField(paintColor, GUILayout.Width(200));
	}
	 
	void ColoredCubesVolumeUpdate(SceneView sceneview)
	{
		Event e = Event.current;
		Ray ray = Camera.current.ScreenPointToRay(new Vector3(e.mousePosition.x, -e.mousePosition.y + Camera.current.pixelHeight));
		Vector3 dir = ray.direction * 1000.0f; //The maximum distance out ray will be cast.
		
		if(coloredCubesVolume)
		{
			if((e.type == EventType.MouseDown) && (e.button == 0))
			{
				// Perform the raycasting. If there's a hit the position will be stored in these ints.
				int resultX, resultY, resultZ;
				if(addMode)
				{
					bool hit = Cubiquity.PickLastEmptyVoxel(coloredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
					if(hit)
					{
						coloredCubesVolume.SetVoxel(resultX, resultY, resultZ, paintColor);
					}
				}
				else if(deleteMode)
				{
					bool hit = Cubiquity.PickFirstSolidVoxel(coloredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
					if(hit)
					{
						coloredCubesVolume.SetVoxel(resultX, resultY, resultZ, new Color32(0, 0, 0, 0));
					}
				}
				else if(paintMode)
				{
					bool hit = Cubiquity.PickFirstSolidVoxel(coloredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
					if(hit)
					{
						coloredCubesVolume.SetVoxel(resultX, resultY, resultZ, paintColor);
					}
				}
			}
			
			coloredCubesVolume.Synchronize();
		}
		else
		{
			Debug.Log("No volume to update");
		}
	}
}