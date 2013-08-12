using UnityEngine;
using System.Collections;

public class RotateTurret : MonoBehaviour
{
	public ColoredCubesVolume coloredCubesVolume;
	
	private float rotationSpeed = 60.0f;

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
		bool hit = Cubiquity.PickFirstSolidVoxel(coloredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
		
		if(hit)
		{		
			// Rotate the turrent to point in the desired direction
			Quaternion currentRotation = transform.rotation;
			Quaternion desiredRotation = Quaternion.LookRotation(new Vector3(resultX, resultY, resultZ) - transform.position);			
			transform.rotation = Quaternion.RotateTowards(currentRotation, desiredRotation, rotationSpeed * Time.deltaTime);
			
			// Now lock the turret to only have rotation in one direction (maybe there's a better way?)
			Vector3 localAngles = transform.localRotation.eulerAngles;
			localAngles.x = 0.0f;
			localAngles.z = 0.0f;
			
			// Apply the restrictyed rotation.
			transform.localRotation = Quaternion.Euler(localAngles);			
		}
	}
}
