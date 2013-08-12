using UnityEngine;
using System.Collections;

public class ElevateCannon : MonoBehaviour
{
	public ColoredCubesVolume coloredCubesVolume;
	
	public float elevationSpeed = 60.0f;
	public float maxElevationAngle = 45.0f;
	public float minElevationAngle = -5.0f;

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
			transform.rotation = Quaternion.RotateTowards(currentRotation, desiredRotation, elevationSpeed * Time.deltaTime);
			
			// Now lock the turret to only have rotation in one direction (maybe there's a better way?)
			Vector3 localAngles = transform.localRotation.eulerAngles;
			localAngles.y = 0.0f;
			localAngles.z = 0.0f;	
			Debug.Log (localAngles.x);
			
			// Also limit the amount of elevation. The 'if' condistion seems a bit inelegant but the wrap
			// around from 360 to 0 happens right in front of the tank. Again, there may be a better way.
			if(localAngles.x > 180.0f)
			{
				// In this case the cannon is elevated.
				localAngles.x = Mathf.Max(localAngles.x, 360.0f - maxElevationAngle);
			}
			else
			{
				// In this case the cannon is pointing down.
				localAngles.x = Mathf.Min(localAngles.x, -minElevationAngle);
			}
			
			// Apply the restrictyed rotation.
			transform.localRotation = Quaternion.Euler(localAngles);
		}
	}
}
