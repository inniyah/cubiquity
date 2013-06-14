using UnityEngine;
using System.Collections;

public class GameLogic : MonoBehaviour
{
	public ColouredCubesVolume colouredCubesVolume;
	public GameObject gun;
	public GameObject mainCamera;
	public float cameraDistance = 10.0f;
	public GameObject tank;
	
	// Initialization
	void Awake()
	{
		GameObject voxelTerrainObject = GameObject.Find("Voxel Terrain");
		colouredCubesVolume = voxelTerrainObject.GetComponent<ColouredCubesVolume>();
		gun = GameObject.Find("Gun");
		mainCamera = GameObject.Find("Main Camera");
		tank = GameObject.Find("Tank");
	}

	// Use this for initialization
	void Start()
	{
		// Create some physics objects for testing.
		/*for(uint x = 12; x < 128; x += 20)
		{
			for(uint z = 12; z < 128; z += 20)
			{
				GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
				cube.AddComponent<Rigidbody>();
				cube.transform.position = new Vector3(x, 30, z);
				cube.transform.RotateAround(new Vector3(x, 12, z), x);
			}
		}*/
		
		// Add some wall to our arena		
		/*DrawCuboid (0, 8, 0, 127, 24, 5, Color.red);
		DrawCuboid (0, 8, 122, 127, 24, 127, Color.red);
		
		DrawCuboid (0, 8, 0, 5, 24, 127, Color.green);		
		DrawCuboid (122, 8, 0, 127, 24, 127, Color.green);*/
	}
	
	// Update is called once per frame
	void Update()
	{		
		cameraDistance -= Input.GetAxis("Mouse ScrollWheel");
		
		//mainCamera.transform = tank.transform;
		//mainCamera.transform.Translate(0.0, 5.0f, 5.0f, Space.Self);
		mainCamera.transform.localPosition = new Vector3(0.0f, cameraDistance * cameraDistance, -cameraDistance);
		mainCamera.transform.LookAt(tank.transform.position + new Vector3(0.0f, 0.0f, -10.0f));
		//Debug.Log (tank.transform.position);
		
		// Build a ray based on the current mouse position
		Vector2 mousePos = Input.mousePosition;
		Ray ray = Camera.main.ScreenPointToRay(new Vector3(mousePos.x, mousePos.y, 0));
		Vector3 dir = ray.direction * 1000.0f; //The maximum distance out ray will be cast.
		
		
		// Perform the raycasting. If there's a hit the position will be stored in these ints.
		int resultX, resultY, resultZ;
		bool hit = Cubiquity.PickVoxel(colouredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
		
		if(hit)
		{
			// If the mouse if over a voxel then turn the turret to face it.	
			gun.transform.LookAt(new Vector3(resultX, resultY, resultZ));
		}
	}
	
	// Draws a cuboid of coloured voxels into our volume.
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
