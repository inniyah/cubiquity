using UnityEngine;
using System.Collections;

public class CameraController : MonoBehaviour
{
	public float cameraDistance = 1.0f;
	public GameObject cameraTarget;
	
	// Use this for initialization
	void Start ()
	{
		cameraTarget = GameObject.Find ("CameraTarget");
	}
	
	// Update is called once per frame
	void Update ()
	{
		cameraDistance -= Input.GetAxis("Mouse ScrollWheel");
		
		cameraTarget.transform.localPosition = new Vector3(0.0f, 0.0f, cameraDistance * 3.0f);
		
		transform.localPosition = new Vector3(0.0f, cameraDistance * cameraDistance, -cameraDistance);
		transform.LookAt(cameraTarget.transform);
	}
}
