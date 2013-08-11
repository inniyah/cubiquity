using UnityEngine;
using UnityEditor;
using System.Collections;
 
[CustomEditor (typeof(ColoredCubesVolume))]
public class ColoredCubesVolumeEditor : Editor
{
	ColoredCubesVolume coloredCubesVolume;
	
	Color paintColor = Color.white;
	
	public void OnEnable()
	{
	    coloredCubesVolume = target as ColoredCubesVolume;
	    SceneView.onSceneGUIDelegate = ColoredCubesVolumeUpdate;
	}
	
	/*public override void OnInspectorGUI()
	{
	    GUILayout.BeginHorizontal();
	    GUILayout.Label(" Grid Width ");
	    grid.width = EditorGUILayout.FloatField(grid.width, GUILayout.Width(50));
	    GUILayout.EndHorizontal();
	}*/
	 
	void ColoredCubesVolumeUpdate(SceneView sceneview)
	{
		Event e = Event.current;
		Ray ray = Camera.current.ScreenPointToRay(new Vector3(e.mousePosition.x, -e.mousePosition.y + Camera.current.pixelHeight));
		Vector3 dir = ray.direction * 1000.0f; //The maximum distance out ray will be cast.
		
		if(coloredCubesVolume)
		{
			if(e.type == EventType.MouseDown)
			{
				// Perform the raycasting. If there's a hit the position will be stored in these ints.
				int resultX, resultY, resultZ;
				bool hit = Cubiquity.PickVoxel(coloredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
				
				if(hit)
				{
					Debug.Log ("Hit");
					coloredCubesVolume.SetVoxel(resultX, resultY, resultZ, paintColor);
				}
				else
				{
					Debug.Log ("Miss");
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