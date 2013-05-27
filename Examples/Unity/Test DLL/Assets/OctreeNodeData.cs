using UnityEngine;
using System.Collections;

[System.Serializable]
public class OctreeNodeData : MonoBehaviour
{
	public int meshLastSyncronised;
	public Vector3 lowerCorner;
	public GameObject[,,] children;
	
	// Use this for initialization
	void Awake()
	{
		//meshLastSyncronised = 0;
		Debug.Log ("Mesh last syncronised = " + meshLastSyncronised);
		hideFlags = HideFlags.HideAndDontSave;
	}
	
	public void OnEnable()
	{
		hideFlags = HideFlags.HideAndDontSave;
	}
	
	// Update is called once per frame
	void Update()
	{	
	}
	
	public GameObject GetChild(uint x, uint y, uint z)
	{
		if(children != null)
		{
			return children[x, y, z];
		}
		else
		{
			return null;
		}
	}
	
	public void SetChild(uint x, uint y, uint z, GameObject gameObject)
	{
		if(children == null)
		{
			children = new GameObject[2, 2, 2];
		}
		
		children[x, y, z] = gameObject;
	}
}
