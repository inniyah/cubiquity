using UnityEngine;
using System.Collections;

public class Shell : MonoBehaviour
{
	public float speed = 200;

	public float range = 400;

	public GameObject ExploPtcl;

	private float dist;
	
	// Update is called once per frame
	void Update ()
	{
		// Move Ball forward
		transform.Translate(Vector3.forward * Time.deltaTime * speed);
		
		// Record Distance.
		dist += Time.deltaTime * speed;
		
		// If reach to my range, Destroy. 
		if(dist >= range)
		{
			Instantiate(ExploPtcl, transform.position, transform.rotation);
			Destroy(gameObject);
		}
	}
	
	void OnTriggerEnter(Collider other)
	{
		// If hit something, Destroy. 
		Instantiate(ExploPtcl, transform.position, transform.rotation);
		Destroy(gameObject);
	}
}
