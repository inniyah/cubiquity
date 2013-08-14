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
	
	static bool forceSelect = false;
	static int forceSelectFrame = 0;
	
	static int myFrameCount = 0;
	
	//private double lastTime = 0.0;
	
	public void OnEnable()
	{
	    coloredCubesVolume = target as ColoredCubesVolume;
		//SceneView.onSceneGUIDelegate -= ColoredCubesVolumeUpdate;
	    //SceneView.onSceneGUIDelegate += ColoredCubesVolumeUpdate;
		
		//SceneView.onSceneGUIDelegate = ColoredCubesVolumeUpdate;
	}
	
	/*public void OnDisable()
	{
		SceneView.onSceneGUIDelegate -= ColoredCubesVolumeUpdate;
	}*/
	
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
	
	public void OnSceneGUI()
	{
		Debug.Log ("ColoredCubesVolumeEditor.OnSceneGUI()");
		Event e = Event.current;
		
		Ray ray = Camera.current.ScreenPointToRay(new Vector3(e.mousePosition.x, -e.mousePosition.y + Camera.current.pixelHeight));
		Vector3 dir = ray.direction * 1000.0f; //The maximum distance out ray will be cast.
		
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
			
			Selection.activeGameObject = coloredCubesVolume.gameObject;
		}
		
		coloredCubesVolume.Synchronize();
	}
	 
	void ColoredCubesVolumeUpdate(SceneView sceneview)
	{
		//Debug.Log ("In ColoredCubesVolumeUpdate " + EditorApplication.timeSinceStartup);
		
		myFrameCount++;
		
		Event e = Event.current;
		
		//Debug.Log (Time.time);
		
		/*Debug.Log (e.ToString());
		Debug.Log (e.type);*/
		
		if(e.type == EventType.used)
		{
			if(forceSelect)
			{
				forceSelect = false;
				forceSelectFrame = 0;
				
				Debug.Log ("Forcing selection");
				Selection.activeGameObject = coloredCubesVolume.gameObject;
			}
		}
		
		
		if(coloredCubesVolume)
		{
			/*if(forceSelect && (myFrameCount > forceSelectFrame))
			{
				forceSelect = false;
				forceSelectFrame = 0;
				
				Debug.Log ("Forcing selection");
				Selection.activeGameObject = coloredCubesVolume.gameObject;
			}*/
			
			Ray ray = Camera.current.ScreenPointToRay(new Vector3(e.mousePosition.x, -e.mousePosition.y + Camera.current.pixelHeight));
			Vector3 dir = ray.direction * 1000.0f; //The maximum distance out ray will be cast.
			
			if(Selection.activeGameObject == coloredCubesVolume.gameObject)
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
				if((e.type == EventType.MouseDown) && (e.button == 0))
				{
					int resultX, resultY, resultZ;
					bool hit = Cubiquity.PickFirstSolidVoxel(coloredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
					if(hit)
					{
						Selection.activeGameObject = coloredCubesVolume.gameObject;
						forceSelect = true;
						forceSelectFrame = myFrameCount + 100;
						Debug.Log("Registering force select at frame " + forceSelectFrame);
					}
				}
			}
		}
	}
}