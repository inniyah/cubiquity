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
		if(GameObject.Find("Voxel Terrain") != null)
		{
			Debug.LogError("A voxel terrain already exists - you (currently) can't create another one.");
			return;
		}
		
		GameObject VoxelTerrainRoot = new GameObject("Voxel Terrain");
		VoxelTerrainRoot.AddComponent<ColouredCubesVolume>();
		
		/*ColouredCubesVolume colouredCubesVolume = VoxelTerrainRoot.GetComponent<ColouredCubesVolume>();
		colouredCubesVolume.Initialize();
		colouredCubesVolume.Synchronize();*/
    }
	
	/*void OnEnable()
	{
		Debug.Log ("ColouredCubesVolumeEditor.OnEnable()");
		
		ColouredCubesVolume colouredCubesVolume = (ColouredCubesVolume)target;
		colouredCubesVolume.Initialize();
		colouredCubesVolume.Synchronize();
	}
	
	void OnDisable()
	{
		Debug.Log ("ColouredCubesVolumeEditor.OnDisable()");
		
		ColouredCubesVolume colouredCubesVolume = (ColouredCubesVolume)target;
		colouredCubesVolume.Shutdown();
	}*/
	
    void OnSceneGUI()
    {
		Debug.Log ("OnSceneGUI");
		/*ColouredCubesVolume colouredCubesVolume = (ColouredCubesVolume)target;
		
		if(colouredCubesVolume)
		{			
			colouredCubesVolume.Synchronize();
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