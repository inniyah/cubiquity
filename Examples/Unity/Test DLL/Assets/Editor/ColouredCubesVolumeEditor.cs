using UnityEngine;
using UnityEditor;
using System.Collections;

[CustomEditor(typeof(ColouredCubesVolume))]
public class ColouredCubesVolumeEditor : Editor
{ 
	//GameObject voxelTerrainObject;
	//ColouredCubesVolume colouredCubesVolume;
	
	[MenuItem("Cubiqity/New Colored Cubes Volume")]
    static void Create()
    {
		GameObject VoxelTerrainRoot = new GameObject("Voxel Terrain");
		VoxelTerrainRoot.AddComponent<ColouredCubesVolume>();
		
		/*ColouredCubesVolume colouredCubesVolume = VoxelTerrainRoot.GetComponent<ColouredCubesVolume>();
		colouredCubesVolume.Initialize();
		colouredCubesVolume.performUpdate();*/
    }
	
	// Use this for initialization
	void OnEnable()
	{
		Debug.Log ("ColouredCubesVolumeEditor.OnEnable()");
		/*voxelTerrainObject = GameObject.Find("Voxel Terrain");
		colouredCubesVolume = voxelTerrainObject.GetComponent<ColouredCubesVolume>();*/
		
		ColouredCubesVolume colouredCubesVolume = (ColouredCubesVolume)target;
		colouredCubesVolume.Initialize();
		colouredCubesVolume.performUpdate();
	}
	
	void OnDisable()
	{
		Debug.Log ("ColouredCubesVolumeEditor.OnDisable()");
		/*voxelTerrainObject = GameObject.Find("Voxel Terrain");
		colouredCubesVolume = voxelTerrainObject.GetComponent<ColouredCubesVolume>();*/
		
		ColouredCubesVolume colouredCubesVolume = (ColouredCubesVolume)target;
		colouredCubesVolume.Shutdown();
	}
	
    void OnSceneGUI()
    {
		Debug.Log ("OnSceneGUI");
		/*ColouredCubesVolume colouredCubesVolume = (ColouredCubesVolume)target;
		
		if(colouredCubesVolume)
		{			
			colouredCubesVolume.performUpdate();
		}
		
        if (Event.current.type == EventType.MouseDown)
        {
			Vector2 mousePos = Event.current.mousePosition;
            Ray ray = Camera.current.ScreenPointToRay(new Vector3(mousePos.x, Camera.current.pixelHeight - mousePos.y, 0));
			
			Vector3 dir = ray.direction * 1000.0f;
			int resultX, resultY, resultZ;
			int hit = CubiquityDLL.cuPickVoxel((uint)((ColouredCubesVolume)target).volumeHandle, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
			
			if(hit == 1)
			{
				Debug.Log("Hit " +resultX + " " + resultY + " " + resultZ);
				
				for(int z = resultZ - 10; z < resultZ + 10; z++)
				{
					for(int y = resultY - 10; y < resultY + 10; y++)
					{
						for(int x = resultX - 10; x < resultX + 10; x++)
						{
							colouredCubesVolume.SetVoxel(x, y, z, new Color32(0,0,0,0));
						}
					}
				}
			}
			else
			{
				Debug.Log("Miss");
			}
        }*/
    }
}