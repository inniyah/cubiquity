using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class ClickToDestroy : MonoBehaviour
{
	private GameObject voxelTerrainObject;
	private ColoredCubesVolume coloredCubesVolume;
	bool isMouseAlreadyDown = false;

	// Use this for initialization
	void Start()
	{
		voxelTerrainObject = gameObject;
		coloredCubesVolume = voxelTerrainObject.GetComponent<ColoredCubesVolume>();
	}
	
	// Update is called once per frame
	void Update()
	{
		if(Input.GetMouseButton(0))
		{
			if(!isMouseAlreadyDown)
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
					Debug.Log ("Hit!");
					int xPos = resultX;
					int yPos = resultY;
					int zPos = resultZ;
					
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
				}
				
				isMouseAlreadyDown = true;
			}
		}
		else
		{
			isMouseAlreadyDown = false;
		}
	}
}
