using UnityEngine;
using System.Collections;

public class ElevateCannon : MonoBehaviour
{
	public ColoredCubesVolume coloredCubesVolume;

	// Use this for initialization
	void Start ()
	{
		GameObject voxelTerrainObject = GameObject.Find("Voxel Terrain");
		coloredCubesVolume = voxelTerrainObject.GetComponent<ColoredCubesVolume>();
	}
	
	// Update is called once per frame
	void Update ()
	{
		// Build a ray based on the current mouse position
		Vector2 mousePos = Input.mousePosition;
		Ray ray = Camera.main.ScreenPointToRay(new Vector3(mousePos.x, mousePos.y, 0));
		Vector3 dir = ray.direction * 1000.0f; //The maximum distance out ray will be cast.
		
		
		// Perform the raycasting. If there's a hit the position will be stored in these ints.
		int resultX, resultY, resultZ;
		bool hit = Cubiquity.PickVoxel(coloredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
		
		if(hit)
		{
			// If the mouse if over a voxel then turn the turret to face it.	
			transform.LookAt(new Vector3(resultX, resultY, resultZ));
			Vector3 euler = transform.eulerAngles;
			euler.y = 0;
			euler.z = 0;
			transform.localEulerAngles = euler;
		}
	}
}
