#pragma strict

var leftTrack : MoveTrack;
var rightTrack : MoveTrack;

var acceleration : float = 5;

var currentVelocity : float = 0;
var maxSpeed : float = 25;

var rotationSpeed : float = 30;

var spawnPoint : Transform;
var bulletObject : GameObject;
var fireEffect : GameObject;

function Start() {

	// Get Track Controls
	leftTrack = GameObject.Find(gameObject.name + "/Lefttrack").GetComponent(MoveTrack);
	rightTrack = GameObject.Find(gameObject.name + "/Righttrack").GetComponent(MoveTrack);
	
}


function Update () 
{
	var vertical : float = Input.GetAxis ("Vertical");
	
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
	transform.Translate(Vector3(0, 0, currentVelocity * Time.deltaTime));

	// Turn tank
	transform.Rotate(Vector3(0, rotationSpeed * Time.deltaTime * Input.GetAxis ("Horizontal"), 0));
	
	
	// Fire!
	if (Input.GetButtonDown("Fire1")) {
		// make fire effect.
		Instantiate(fireEffect, spawnPoint.position, spawnPoint.rotation);
		
		// make ball
		Instantiate(bulletObject, spawnPoint.position, spawnPoint.rotation);
	}

}