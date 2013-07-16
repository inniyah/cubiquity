using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class Shell : MonoBehaviour
{
	public float speed = 50;

	public float range = 400;

	public GameObject ExploPtcl;

	private float dist;
	
	private GameObject voxelTerrainObject;
	private ColoredCubesVolume coloredCubesVolume;
	
	void Awake()
	{
		voxelTerrainObject = GameObject.Find("Voxel Terrain");
		coloredCubesVolume = voxelTerrainObject.GetComponent<ColoredCubesVolume>();
	}
	
	// Update is called once per frame
	void Update ()
	{
		float desiredDistanceThisUpdate = Time.deltaTime * speed;
		
		float actualDistanceThisUpdate = 0.0f;
		
		while(actualDistanceThisUpdate < desiredDistanceThisUpdate)
		{
			const float step = 0.5f;
			
			// Move Ball forward
			transform.Translate(Vector3.forward * step);
			
			// Record Distance.
			dist += step;
			
			// If reach to my range, Destroy. 
			if(dist >= range)
			{
				Instantiate(ExploPtcl, transform.position, transform.rotation);
				Destroy(gameObject);
				break;
			}
			
			// Test for hit
			int xPos = (int)transform.position.x;
			int yPos = (int)transform.position.y;
			int zPos = (int)transform.position.z;
			
			Color32 centerColor = coloredCubesVolume.GetVoxel(xPos, yPos, zPos);
			
			if(centerColor.a > 127)
			{
				createExplosion();
				break;
			}
			
			actualDistanceThisUpdate += step;
		}
	}
	
	void createExplosion()
	{
		// If hit something, Destroy. 
		Instantiate(ExploPtcl, transform.position, transform.rotation);
		
		int xPos = (int)transform.position.x;
		int yPos = (int)transform.position.y;
		int zPos = (int)transform.position.z;
		
		Vector3 pos = new Vector3(xPos, yPos, zPos);
		
		List<IntVector3> voxelsToDelete = new List<IntVector3>();
		
		for(int z = zPos - 5; z < zPos + 5; z++)
		{
			for(int y = yPos - 5; y < yPos + 5; y++)
			{
				for(int x = xPos - 5; x < xPos + 5; x++)
				{					
					int xDiff = x - xPos;
					int yDiff = y - yPos;
					int zDiff = z - zPos;
					
					int distSquared = xDiff * xDiff + yDiff * yDiff + zDiff * zDiff;
					
					if(distSquared < 25)
					{	
						Color32 color = coloredCubesVolume.GetVoxel(x, y, z);				
						
						if(color.a > 127)
						{
							bool isSurfaceVoxel = coloredCubesVolume.IsSurfaceVoxel(x, y, z); //Save this before we clear the voxel.
							//coloredCubesVolume.SetVoxel(x, y, z, new Color32(0,0,0,0));
							
							IntVector3 voxel = new IntVector3(x, y, z);
							voxelsToDelete.Add(voxel);
						
							//if(distSquared > 12)
							{
								//if((x+y+z)% 2 == 0)
								if(isSurfaceVoxel)
								{
									GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
									cube.AddComponent<Rigidbody>();
									cube.AddComponent<SeparatedVoxel>();
									cube.transform.position = new Vector3(x, y, z);
									cube.transform.localScale = new Vector3(0.9f, 0.9f, 0.9f);
									cube.renderer.material.color = color;
									cube.layer = LayerMask.NameToLayer("Cube");
									
									Vector3 explosionForce = cube.transform.position - pos;
									
									float xTorque = (x * 1436523.4f) % 56.0f;
									float yTorque = (y * 56143.4f) % 43.0f;
									float zTorque = (z * 22873.4f) % 38.0f;
									
									Vector3 up = new Vector3(0.0f, 2.0f, 0.0f);
									
									cube.rigidbody.AddTorque(xTorque, yTorque, zTorque);
									cube.rigidbody.AddForce((explosionForce.normalized + up) * 100.0f);
									
									//Object.Destroy(cube, 5.0f);
								}
							}
						}
					}
				}
			}
		}
		
		foreach (IntVector3 voxel in voxelsToDelete) // Loop through List with foreach
		{
		    coloredCubesVolume.SetVoxel(voxel.x, voxel.y, voxel.z, new Color32(0,0,0,0));
		}
		
		Destroy(gameObject);
	}
}
