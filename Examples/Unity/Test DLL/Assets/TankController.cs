using UnityEngine;
using System.Collections;

public class TankController : MonoBehaviour
{
	public float Speed = 50;
	public float TurnSpeed = 2;
	
	private bool isMouseDown;
	
	public GameObject gun;
	
	void Awake()
	{
		gun = GameObject.Find("Gun");
	}
	 
	// Use this for initialization
	void Start ()
	{
	
	}
	
	// Update is called once per frame
	void Update ()
	{
		float forwardForce = Input.GetAxis("Vertical") * Speed;
		float turnAmount = Input.GetAxis("Horizontal") * TurnSpeed;
		
		transform.Rotate(0, turnAmount, 0);
		rigidbody.AddRelativeForce(0, 0, forwardForce);
		
		if (Input.GetMouseButton(0))
        {
			if(!isMouseDown) // The flag is not set so the mouse has only just gone down. (i.e. a click).
			{
				//Debug.Log("Got mouse down!");
				GameObject bullet = GameObject.CreatePrimitive(PrimitiveType.Sphere);
				bullet.AddComponent<Bullet>();	
				bullet.AddComponent<Rigidbody>();
				//bullet.AddComponent<Collider>();
				
				bullet.transform.position = gun.transform.position;
				bullet.transform.rotation = gun.transform.rotation;
				
				bullet.transform.Translate(0.0f, 0.0f, 3.0f, Space.Self);
				
				bullet.rigidbody.AddForce(gun.transform.forward * 50.0f,ForceMode.Impulse);
				
				Object.Destroy(bullet, 5.0f);
				
				isMouseDown = true;
			}
		}
		else
		{
			isMouseDown = false;
		}
	}
}
