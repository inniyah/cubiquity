using UnityEngine;
using System.Collections;

[System.Serializable]
public class GameLogic : MonoBehaviour
{
	//uint frameCounter;
	public GameObject voxelTerrainObject;
	public ColouredCubesVolume colouredCubesVolume;
	public GameObject gun;
	
	void Awake()
	{
		voxelTerrainObject = GameObject.Find("Voxel Terrain");
		colouredCubesVolume = voxelTerrainObject.GetComponent<ColouredCubesVolume>();
		gun = GameObject.Find("Gun");
	}

	// Use this for initialization
	void Start()
	{
		for(uint x = 12; x < 128; x += 20)
		{
			for(uint z = 12; z < 128; z += 20)
			{
				GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
				cube.AddComponent<Rigidbody>();
				cube.transform.position = new Vector3(x, 30, z);
				cube.transform.RotateAround(new Vector3(x, 12, z), x);
			}
		}
		
		/*DrawCuboid(0, 0, 0, (int)colouredCubesVolume.Width - 1, 8, (int)colouredCubesVolume.Depth, Color.gray);
		
		DrawCuboid (0, 9, 0, 127, 24, 5, Color.red);
		DrawCuboid (0, 9, 122, 127, 24, 127, Color.red);
		
		DrawCuboid (0, 9, 0, 5, 24, 127, Color.green);		
		DrawCuboid (122, 9, 0, 127, 24, 127, Color.green);*/
	}
	
	// Update is called once per frame
	void Update()
	{
		Vector2 mousePos = Input.mousePosition;

		Ray ray = Camera.main.ScreenPointToRay(new Vector3(mousePos.x, mousePos.y, 0));
		Vector3 dir = ray.direction * 1000.0f;
		int resultX, resultY, resultZ;
		int hit = CubiquityDLL.cuPickVoxel((uint)colouredCubesVolume.volumeHandle, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
		
		if(hit == 1)
		{
			//Debug.Log("Hit " +resultX + " " + resultY + " " + resultZ);			
			gun.transform.LookAt(new Vector3(resultX, resultY, resultZ));
		}

	}
	
	void DrawCuboid(int lowerX, int lowerY, int lowerZ, int upperX, int upperY, int upperZ, Color32 color)
	{
		for(int z = lowerZ; z <= upperZ; z++)
		{
			for(int y = lowerY; y <= upperY; y++)
			{
				for(int x = lowerX; x <= upperX; x++)
				{
					colouredCubesVolume.SetVoxel(x, y, z, color);
				}
			}
		}
	}
}
