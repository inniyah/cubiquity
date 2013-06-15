using UnityEngine;
using System.Collections;

public class SeparatedVoxel : MonoBehaviour
{
	private float creationTime;
	private float lifeTime = 10.0f;
	private static float fadeTime = 2.0f;
	private bool fading = false;
	private float fadeStart;

	// Use this for initialization
	void Start()
	{	
		creationTime = Time.time;
		
		Random random = new Random();
		lifeTime = Random.Range(8.0f, 12.0f);
	}
	
	// Update is called once per frame
	void Update()
	{	
		if(!fading)
		{
			if(Time.time > creationTime + lifeTime)
			{
				Destroy (rigidbody);
				Destroy (collider);
				fading = true;
				fadeStart = Time.time;
				
				renderer.material.shader = Shader.Find ("Transparent/Diffuse");
			}
		}
		
		if(fading)
		{
			float timeSpentFading = Time.time - fadeStart;
			
			Color color = renderer.material.color;
			color.a = 1.0f - timeSpentFading / fadeTime;
			renderer.material.color = color;
			
			if(color.a < 0.01)
			{
				Destroy (gameObject);
			}
		}
	}
}
