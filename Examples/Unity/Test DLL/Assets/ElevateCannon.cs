using UnityEngine;
using System.Collections;

public class ElevateCannon : MonoBehaviour
{
	public ColoredCubesVolume coloredCubesVolume;
	
	private float elevationSpeed = 60.0f;

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
			// Rotate the turrent to point in the desired direction
			Quaternion currentRotation = transform.rotation;
			Quaternion desiredRotation = Quaternion.LookRotation(new Vector3(resultX, resultY, resultZ) - transform.position);			
			transform.rotation = Quaternion.RotateTowards(currentRotation, desiredRotation, elevationSpeed * Time.deltaTime);
			
			// Now lock the turret to only have rotation in one direction (maybe there's a better way?)
			Vector3 localAngles = transform.localRotation.eulerAngles;
			localAngles.y = 0.0f;
			localAngles.z = 0.0f;	
			Debug.Log (localAngles.x);
			
			// Also limit the amount of elevation.
			if(localAngles.x > 180.0f)
			{
				localAngles.x = Mathf.Max(localAngles.x, 360.0f - 45.0f);
			}
			else
			{
				localAngles.x = Mathf.Min(localAngles.x, 5.0f);
			}
			//localAngles.x = Mathf.Min(localAngles.x, 45.0f);
			//localAngles.x = Mathf.Max(localAngles.x, -5.0f);
			
			transform.localRotation = Quaternion.Euler(localAngles);
		}
	}
}
