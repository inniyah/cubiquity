using UnityEngine;
using System.Collections;

public class Bullet : MonoBehaviour
{
	public GameObject voxelTerrainObject;
	public ColouredCubesVolume colouredCubesVolume;
	
	void Awake()
	{
		voxelTerrainObject = GameObject.Find("Voxel Terrain");
		colouredCubesVolume = voxelTerrainObject.GetComponent<ColouredCubesVolume>();
	}

	// Use this for initialization
	void Start()
	{	
	}
	
	// Update is called once per frame
	void Update()
	{
		float bulletSpeed = 100.0f;
		transform.Translate(0.0f, 0.0f, Time.deltaTime * bulletSpeed, Space.Self);
		
		int xPos = (int)transform.position.x;
		int yPos = (int)transform.position.y;
		int zPos = (int)transform.position.z;
		
		
		
		Color32 centerColor = colouredCubesVolume.GetVoxel(xPos, yPos, zPos);
		
		if(centerColor.a > 127)
		{
			Debug.Log ("Impact!");
			
			transform.Translate(0.0f, 0.0f, 2.0f, Space.Self);
		
			xPos = (int)transform.position.x;
			yPos = (int)transform.position.y;
			zPos = (int)transform.position.z;
			
			Vector3 pos = new Vector3(xPos, yPos, zPos);
			
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
							Color32 color = colouredCubesVolume.GetVoxel(x, y, z);				
							
							if(color.a > 127)
							{
								colouredCubesVolume.SetVoxel(x, y, z, new Color32(0,0,0,0));
							
								//if(distSquared > 12)
								{
									//if((x+y+z)% 2 == 0)
									{
										GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
										cube.AddComponent<Rigidbody>();
										cube.transform.position = new Vector3(x, y, z);
										cube.transform.localScale = new Vector3(0.9f, 0.9f, 0.9f);
										cube.renderer.material.color = color;
										
										Vector3 explosionForce = cube.transform.position - pos;
										
										cube.rigidbody.AddTorque(12.0f, 21.3f, 13.4f);
										cube.rigidbody.AddForce(explosionForce.normalized * 100.0f);
									}
								}
							}
						}
					}
				}
			}
			
			Object.Destroy(this.gameObject);
		}
	}
	
	/*void OnCollisionEnter(Collision collision)
	{
		//Vector3 explosionCentre = transform.position + (rigidbody.velocity * 1.0f);
		
		ContactPoint contactPoint = collision.contacts[0];
		Vector3 contactPosition = contactPoint.point;
		
		int xPos = (int)(contactPosition.x + 0.5f);
		int yPos = (int)(contactPosition.y + 0.5f);
		int zPos = (int)(contactPosition.z + 0.5f);	
		
		//Vector3 forceDirection = cube.transform.position - collision.;
						
		//Debug.Log(forceDirection);
		
		Color32 emptyVoxelColor = new Color32(0, 0, 0, 0);
			
		for(int z = zPos - 2; z < zPos + 2; z++)
		{
			for(int y = yPos - 2; y < yPos + 2; y++)
			{
				for(int x = xPos - 2; x < xPos + 2; x++)
				{
					Color32 color = colouredCubesVolume.GetVoxel(x, y, z);
					if(color.a > 127)
					{					
						colouredCubesVolume.SetVoxel(x, y, z, emptyVoxelColor);
						
						GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
						cube.AddComponent<Rigidbody>();
						cube.transform.position = new Vector3(x, y, z);
						cube.transform.localScale = new Vector3(0.2f, 0.2f, 0.2f);
						cube.renderer.material.color = color;						
						
						cube.rigidbody.AddTorque(12.0f, 21.3f, 13.4f);
						cube.rigidbody.AddForce(contactPoint.normal * 100.0f);
					}
				}
			}
		}
		
		Object.Destroy(this.gameObject);
	}*/
}
