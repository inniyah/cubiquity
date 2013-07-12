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
			Vector3 toTarget = new Vector3(resultX, resultY, resultZ) - transform.position;			
			Quaternion q = Quaternion.FromToRotation(/*hingeJoint.connectedBody.transform.forward*/ new Vector3(0.0f, 0.0f, 1.0f) , toTarget);
			
			JointSpring spr = hingeJoint.spring;
			spr.targetPosition = q.eulerAngles.x;
			hingeJoint.spring = spr;
		}
	}
}
