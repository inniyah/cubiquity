using UnityEngine;
using System.Collections;

public class ShowBounds : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}
	
	public bool _bIsSelected = true;
 
  void OnDrawGizmos()
  {
    if (_bIsSelected)
      OnDrawGizmosSelected();
  }
 
 
  void OnDrawGizmosSelected()
  {
    Gizmos.color = Color.yellow;
    Gizmos.DrawSphere(transform.position, 1.0f);  //center sphere
    if (transform.renderer != null)
      Gizmos.DrawWireCube(transform.position, transform.renderer.bounds.size);
  }
}
