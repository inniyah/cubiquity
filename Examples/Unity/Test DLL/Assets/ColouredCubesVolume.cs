using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Text;

public struct MyVertex 
{
	public float x;
	public float y;
	public float z;
	public UInt32 colour;
}

[ExecuteInEditMode]
public class ColouredCubesVolume : MonoBehaviour
{
	public string folderName;
	
	internal uint? volumeHandle = null;
	private GameObject rootGameObject;
	
	public void Initialize()
	{	
		// I don't understand why we need to do this. If it's a new oject it shouldn't have any children,
		// or if it's a reused object the children should have been removed by Shutdown(). But when switching
		// from editor mode to play mode the children don't seem to be removed properly.
		foreach(Transform child in transform)
		{
			Debug.Log("Removing existing child from game object.");
			
			//Deleting while in a loop - is this valid?
			DestroyImmediate(child.gameObject);
		}

		// Use the Cubiquity dll to allocate some volume data
		if((folderName != null) && (folderName != ""))
		{
			volumeHandle = CubiquityDLL.NewColouredCubesVolumeFromVolDat(folderName, 64, 64);
		}
		else
		{
			volumeHandle = CubiquityDLL.NewColouredCubesVolume(0, 0, 0, 127, 31, 127, 64, 64);
			
			// Set some voxels to solid so the user can see the volume they just created.
			Color32 lightGrey = new Color32(192, 192, 192, 255);
			for(int z = 0; z < 128; z++)
			{
				for(int y = 0; y < 8; y++)
				{
					for(int x = 0; x < 128; x++)
					{
						SetVoxel(x, y, z, lightGrey);
					}
				}
			}
		}
	}
	
	public void Synchronize()
	{
		if(volumeHandle.HasValue)
		{
			CubiquityDLL.UpdateVolume(volumeHandle.Value);
			
			if(CubiquityDLL.HasRootOctreeNode(volumeHandle.Value) == 1)
			{		
				uint rootNodeHandle = CubiquityDLL.GetRootOctreeNode(volumeHandle.Value);
			
				if(rootGameObject == null)
				{					
					rootGameObject = BuildGameObjectFromNodeHandle(rootNodeHandle, gameObject);	
				}
				syncNode(rootNodeHandle, rootGameObject);
			}
		}
	}
	
	public void Shutdown()
	{
		Debug.Log("In ColouredCubesVolume.Shutdown()");
		
		if(volumeHandle.HasValue)
		{
			CubiquityDLL.DeleteColouredCubesVolume(volumeHandle.Value);
			volumeHandle = null;
		
			deleteGameObject(rootGameObject);
		}
	}
	
	public void deleteGameObject(GameObject gameObjectToDelete)
	{
		MeshFilter mf = (MeshFilter)gameObjectToDelete.GetComponent(typeof(MeshFilter));
		DestroyImmediate(mf.sharedMesh);
		
		OctreeNodeData octreeNodeData = gameObjectToDelete.GetComponent<OctreeNodeData>();
		
		//Now delete any children
		for(uint z = 0; z < 2; z++)
		{
			for(uint y = 0; y < 2; y++)
			{
				for(uint x = 0; x < 2; x++)
				{
					GameObject childObject = octreeNodeData.GetChild(x, y, z);
					if(childObject != null)
					{
						deleteGameObject(childObject);
					}
				}
			}
		}
		
		DestroyImmediate(gameObjectToDelete);
	}
	
	void Start()
	{
		Debug.Log ("ColouredCubesVolume.Awake()");
		Initialize();
	}
	
	// Use this for initialization
	/*void Start()
	{		
		
	}*/
	
	// Update is called once per frame
	void Update()
	{
		Synchronize();
	}
	
	public void OnDestroy()
	{
		Debug.Log ("ColouredCubesVolume.OnDestroy()");
		Shutdown();
	}
	
	public Color32 GetVoxel(int x, int y, int z)
	{
		Color32 color = new Color32();
		if(volumeHandle.HasValue)
		{
			CubiquityDLL.GetVoxel(volumeHandle.Value, x, y, z, out color.r, out color.g, out color.b, out color.a);
		}
		return color;
	}
	
	public void SetVoxel(int x, int y, int z, Color32 color)
	{
		if(volumeHandle.HasValue)
		{
			if(x >= 0 && y >= 0 && z >= 0 && x < 128 && y < 32 && z < 128) // FIX THESE VALUES!
			{
				byte alpha = color.a > 127 ? (byte)255 : (byte)0; // Threshold the alpha until we support transparency.
				CubiquityDLL.SetVoxel(volumeHandle.Value, x, y, z, color.r, color.g, color.b, alpha);
			}
		}
	}
	
	public void syncNode(uint nodeHandle, GameObject gameObjectToSync)
	{
		uint meshLastUpdated = CubiquityDLL.GetMeshLastUpdated(nodeHandle);		
		OctreeNodeData octreeNodeData = (OctreeNodeData)(gameObjectToSync.GetComponent<OctreeNodeData>());
		
		if(octreeNodeData.meshLastSyncronised < meshLastUpdated)
		{			
			if(CubiquityDLL.NodeHasMesh(nodeHandle) == 1)
			{				
				Mesh mesh;
				Mesh physicsMesh;
				
				BuildMeshFromNodeHandle(nodeHandle, out mesh, out physicsMesh);
		
		        MeshFilter mf = (MeshFilter)gameObjectToSync.GetComponent(typeof(MeshFilter));
		        MeshRenderer mr = (MeshRenderer)gameObjectToSync.GetComponent(typeof(MeshRenderer));
				MeshCollider mc = (MeshCollider)gameObjectToSync.GetComponent(typeof(MeshCollider));
				
				if(mf.sharedMesh != null)
				{
					DestroyImmediate(mf.sharedMesh);
				}
				
		        mf.sharedMesh = mesh;
				mc.sharedMesh = physicsMesh;
				
				mr.material = new Material(Shader.Find("ColoredCubesVolume"));
				
				//mc.material.bounciness = 1.0f;				
				//mr.renderer.material.shader = Shader.Find("ColouredCubesVolume");
			}
			
			uint currentTime = CubiquityDLL.GetCurrentTime();
			octreeNodeData.meshLastSyncronised = (int)(currentTime);
		}		
		
		//Now syncronise any children
		for(uint z = 0; z < 2; z++)
		{
			for(uint y = 0; y < 2; y++)
			{
				for(uint x = 0; x < 2; x++)
				{
					if(CubiquityDLL.HasChildNode(nodeHandle, x, y, z) == 1)
					{					
					
						uint childNodeHandle = CubiquityDLL.GetChildNode(nodeHandle, x, y, z);					
						
						GameObject childGameObject = octreeNodeData.GetChild(x,y,z);
						
						if(childGameObject == null)
						{							
							childGameObject = BuildGameObjectFromNodeHandle(childNodeHandle, gameObjectToSync);
							
							octreeNodeData.SetChild(x, y, z, childGameObject);
						}
						
						syncNode(childNodeHandle, childGameObject);
					}
				}
			}
		}
	}
	
	GameObject BuildGameObjectFromNodeHandle(uint nodeHandle, GameObject parentGameObject)
	{
		int xPos, yPos, zPos;
		//Debug.Log("Getting position for node handle = " + nodeHandle);
		CubiquityDLL.GetNodePosition(nodeHandle, out xPos, out yPos, out zPos);
		
		StringBuilder name = new StringBuilder("(" + xPos + ", " + yPos + ", " + zPos + ")");
		
		GameObject newGameObject = new GameObject(name.ToString ());
		newGameObject.AddComponent<OctreeNodeData>();
		newGameObject.AddComponent<MeshFilter>();
		newGameObject.AddComponent<MeshRenderer>();
		newGameObject.AddComponent<MeshCollider>();
		
		OctreeNodeData octreeNodeData = newGameObject.GetComponent<OctreeNodeData>();
		octreeNodeData.lowerCorner = new Vector3(xPos, yPos, zPos);
		
		newGameObject.transform.parent = parentGameObject.transform;
		
		if(parentGameObject != gameObject)
		{
			Vector3 parentLowerCorner = parentGameObject.GetComponent<OctreeNodeData>().lowerCorner;
			newGameObject.transform.localPosition = octreeNodeData.lowerCorner - parentLowerCorner;
		}
		else
		{
			newGameObject.transform.localPosition = octreeNodeData.lowerCorner;
		}
		
		return newGameObject;
	}
	
	float packPosition(float x, float y, float z)
	{
		x += 0.5f;
		y += 0.5f;
		z += 0.5f;
		
		if((x < 0.0f) || (x > 100.0f))
		{
			Debug.Log ("x is " + x);
		}
		
		float result = x * 65536.0f + y * 256.0f + z;
		
		return result;
	}
	
	void BuildMeshFromNodeHandle(uint nodeHandle, out Mesh mesh, out Mesh physicsMesh)
	{
		uint noOfVertices = CubiquityDLL.GetNoOfVertices(nodeHandle);
		uint noOfIndices = CubiquityDLL.GetNoOfIndices(nodeHandle);
		
		IntPtr ptrResultVerts = CubiquityDLL.GetVertices(nodeHandle);
		
		// Load the results into a managed array. 
		uint floatsPerVert = 4;
		int resultVertLength = (int)(noOfVertices * floatsPerVert);
        float[] resultVertices = new float[resultVertLength];
        Marshal.Copy(ptrResultVerts
            , resultVertices
            , 0
            , resultVertLength);
		
		MyVertex[] myVertices = new MyVertex[noOfVertices];
		
		for (int i = 0; i < noOfVertices; i++)
    	{
			myVertices[i] = (MyVertex)Marshal.PtrToStructure(ptrResultVerts, typeof(MyVertex));
			ptrResultVerts = new IntPtr(ptrResultVerts.ToInt64() + Marshal.SizeOf(typeof(MyVertex)));
		}
		
		//Build a mesh procedurally
		
		IntPtr ptrResultIndices = CubiquityDLL.GetIndices(nodeHandle);
		
		// Load the results into a managed array.
        int[] resultIndices = new int[noOfIndices]; //Should be unsigned!
        Marshal.Copy(ptrResultIndices
            , resultIndices
            , 0
            , (int)noOfIndices);
		
		mesh = new Mesh();
        //mesh.name = "testMesh";
        mesh.Clear();		
		
		physicsMesh = new Mesh();
		
		Vector3[] physicsVertices = new Vector3[resultVertLength / 4];
		
        Vector3[] vertices = new Vector3[resultVertLength / 4];
		Vector2[] uv = new Vector2[resultVertLength / 4];
		for(int ct = 0; ct < resultVertLength / 4; ct++)
		{
			UInt32 colour = (UInt32)myVertices[ct].colour;
			UInt32 red = (UInt32)((colour >> 0) & 0xF) * 16;
			UInt32 green = (UInt32)((colour >> 4) & 0xF) * 16;
			UInt32 blue = (UInt32)((colour >> 8) & 0xF) * 16;
			//UInt32 alpha = (UInt32)((colour >> 12) & 0xF) * 16;
			
			float colourAsFloat = (float)(red * 65536 + green * 256 + blue);
			
			//vertices[ct] = new Vector3(resultVertices[ct * 4 + 0], resultVertices[ct * 4 + 1], resultVertices[ct * 4 + 2]);			
			physicsVertices[ct] = new Vector3(myVertices[ct].x, myVertices[ct].y, myVertices[ct].z);
			
			float packedPosition = packPosition(myVertices[ct].x, myVertices[ct].y, myVertices[ct].z);
			vertices[ct] = new Vector3(packedPosition, colourAsFloat, 0.0f);
			
			
			//float colourAsFloat = (float)green / 15.0f;
			
			uv[ct] = new Vector2(colourAsFloat, colourAsFloat);
			//uv[ct] = new Vector2(0.0f, 0.0f);
		}
		mesh.vertices = vertices; 
		mesh.triangles = resultIndices;
		mesh.uv = uv;
		
		// FIXME - Get proper bounds
		mesh.bounds = new Bounds(new Vector3(0.0f, 0.0f, 0.0f), new Vector3(500.0f, 500.0f, 500.0f));
		
		physicsMesh.vertices = physicsVertices;
		physicsMesh.triangles = resultIndices;
		
		//return mesh;
	}
}
