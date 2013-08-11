using UnityEngine;
using System.Collections;

public class MoveTank : MonoBehaviour
{
	public float acceleration = 5;

	public float currentVelocity = 0;
	public float maxSpeed = 25;
	
	public float rotationSpeed = 30;
	
	public Transform spawnPoint;
	public GameObject bulletObject;
	public GameObject fireEffect;

	// Use this for initialization
	void Start ()
	{
	
	}
	
	// Update is called once per frame
	void Update ()
	{
		float vertical = Input.GetAxis ("Vertical");
	
		currentVelocity += acceleration * Time.deltaTime * vertical;
		
		if(Mathf.Abs(vertical) < 0.01f)
		{
			// No key input. 
			if (currentVelocity > 0) 
				currentVelocity -= acceleration * Time.deltaTime;
			else if (currentVelocity < 0) 
				currentVelocity += acceleration * Time.deltaTime;
		}
	
		// Turn off engine if currentVelocity is too small. 
		if (Mathf.Abs(currentVelocity) <= 0.05)
			currentVelocity = 0;
	
		// Move Tank by currentVelocity
		transform.Translate(new Vector3(0, 0, currentVelocity * Time.deltaTime));
	
		// Turn tank
		transform.Rotate(new Vector3(0, rotationSpeed * Time.deltaTime * Input.GetAxis ("Horizontal"), 0));
		
		
		// Fire!
		if ((GUIUtility.hotControl == 0) && (Input.GetButtonDown("Fire1"))) {
			// make fire effect.
			Instantiate(fireEffect, spawnPoint.position, spawnPoint.rotation);
			
			// make ball
			Instantiate(bulletObject, spawnPoint.position, spawnPoint.rotation);
		}
	}
}
