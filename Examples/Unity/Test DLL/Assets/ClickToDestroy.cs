using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class ClickToDestroy : MonoBehaviour
{
	private ColoredCubesVolume coloredCubesVolume;
	
	// Bit of a hack - we want to detect mouse clicks rather than the mouse simply being down,
	// but we can't use OnMouseDown because the voxel terrain doesn't have a collider (the
	// individual pieces do, but not the parent). So we define a click as the mouse being down
	// but not being down on the previous frame. We'll fix this better in the future...
	private bool isMouseAlreadyDown = false;

	// Use this for initialization
	void Start()
	{
		// We'll store a reference to the colored cubes volume so we can interact with it later.
		coloredCubesVolume = gameObject.GetComponent<ColoredCubesVolume>();
		if(coloredCubesVolume == null)
		{
			Debug.LogError("This 'ClickToDestroy' script should be attached to a game object with a ColoredCubesVolume component");
		}
	}
	
	// Update is called once per frame
	void Update()
	{
		// Bail out if we're not attached to a terrain.
		if(coloredCubesVolume == null)
		{
			return;
		}
		
		// If the mouse btton is down and it was not down last frame
		// then we consider this a click, and do our destruction.
		if(Input.GetMouseButton(0))
		{
			if(!isMouseAlreadyDown)
			{
				// Build a ray based on the current mouse position
				Vector2 mousePos = Input.mousePosition;
				Ray ray = Camera.main.ScreenPointToRay(new Vector3(mousePos.x, mousePos.y, 0));
				Vector3 dir = ray.direction * 1000.0f; //The maximum distance our ray will be cast.
				
				
				// Perform the raycasting. If there's a hit the position will be stored in these ints.
				int resultX, resultY, resultZ;
				bool hit = Cubiquity.PickFirstSolidVoxel(coloredCubesVolume, ray.origin.x, ray.origin.y, ray.origin.z, dir.x, dir.y, dir.z, out resultX, out resultY, out resultZ);
				
				// If we hit a solid voxel then create an explosion at this point.
				if(hit)
				{					
					int range = 5;
					DestroyVoxels(resultX, resultY, resultZ, range);
				}
				
				// Set this flag so the click won't be processed again next frame.
				isMouseAlreadyDown = true;
			}
		}
		else
		{
			// Clear the flag while we wait for a click.
			isMouseAlreadyDown = false;
		}
	}
	
	void DestroyVoxels(int xPos, int yPos, int zPos, int range)
	{
		// Initialise outside the loop, but we'll use it later.
		Vector3 pos = new Vector3(xPos, yPos, zPos);
		
		
		List<IntVector3> voxelsToDelete = new List<IntVector3>();
		
		for(int z = zPos - range; z < zPos + range; z++)
		{
			for(int y = yPos - range; y < yPos + range; y++)
			{
				for(int x = xPos - range; x < xPos + range; x++)
				{					
					int xDiff = x - xPos;
					int yDiff = y - yPos;
					int zDiff = z - zPos;
					
					int rangeSquared = range * range;
					int distSquared = xDiff * xDiff + yDiff * yDiff + zDiff * zDiff;
					
					if(distSquared < rangeSquared)
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
									
									// These are basically random values found through experimentation.
									// They just add a bit of twist as the cubes explode which looks nice
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
}
