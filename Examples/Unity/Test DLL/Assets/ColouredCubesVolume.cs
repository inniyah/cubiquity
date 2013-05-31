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

public class ColouredCubesVolume : MonoBehaviour
{
	public uint volumeHandle = 0;
	public bool hasVolumeHandle = false;
	public GameObject rootGameObject;
	
	public Material colouredCubesMaterial;
	
	public void Initialize()
	{		
		if(rootGameObject != null)
		{
			deleteGameObject(rootGameObject);
		}
		
		colouredCubesMaterial = new Material(Shader.Find("ColouredCubesVolume"));
			
        Debug.Log("In ColouredCubesVolume.Initialize()");

        if (hasVolumeHandle == false)
        {
			uint volHand = CubiquityDLL.NewColouredCubesVolumeFromVolDat(new StringBuilder("C:/Code/cubiquity/Examples/SliceData/VoxeliensTerrain/"), 64, 64);
			volumeHandle = volHand;
			hasVolumeHandle = true;
            Debug.Log("Created volume: handle = " + volumeHandle);
        }
        else
        {
            Debug.Log("ERROR - VOLUMR HANDLE ALREADY EXISTS");
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
	
	public void performUpdate()
	{
		//Debug.Log ("performUpdate");
		if(hasVolumeHandle)
		{
			CubiquityDLL.UpdateVolume((uint)volumeHandle);
			
			if(CubiquityDLL.HasRootOctreeNode((uint)volumeHandle) == 1)
			{		
				uint rootNodeHandle = CubiquityDLL.GetRootOctreeNode((uint)volumeHandle);
			
				if(rootGameObject == null)
				{
					Debug.Log ("Creating Root GameObject");
					
					rootGameObject = BuildGameObjectFromNodeHandle(rootNodeHandle, gameObject);	
					//rootGameObject = new GameObject("My GameObject");
				}
				syncNode(rootNodeHandle, rootGameObject);
			}
		}
	}
	
	public void Shutdown()
	{
		Debug.Log("In ColouredCubesVolume.Shutdown()");
		
		CubiquityDLL.DeleteColouredCubesVolume((uint)volumeHandle);
		volumeHandle = 0;
		hasVolumeHandle = false;
		
		deleteGameObject(rootGameObject);
	}
	
	void Awake()
	{
		Initialize();
	}
	
	// Use this for initialization
	void Start()
	{		
		
	}
	
	// Update is called once per frame
	void Update()
	{
		performUpdate();
	}
	
	public void OnDestroy()
	{
		Shutdown();
	}
	
	public Color32 GetVoxel(int x, int y, int z)
	{
		Color32 color = new Color32();
		if(hasVolumeHandle)
		{
			CubiquityDLL.GetVoxel((uint)volumeHandle, x, y, z, out color.r, out color.g, out color.b, out color.a);
		}
		return color;
	}
	
	public void SetVoxel(int x, int y, int z, Color32 color)
	{
		if(hasVolumeHandle)
		{
			if(x >= 0 && y >= 0 && z >= 0 && x < 128 && y < 32 && z < 128) // FIX THESE VALUES!
			{
				byte alpha = color.a > 127 ? (byte)255 : (byte)0; // Threshold the alpha until we support transparency.
				CubiquityDLL.SetVoxel((uint)volumeHandle, x, y, z, color.r, color.g, color.b, alpha);
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
				Mesh mesh = BuildMeshFromNodeHandle(nodeHandle);
		
		        MeshFilter mf = (MeshFilter)gameObjectToSync.GetComponent(typeof(MeshFilter));
		        MeshRenderer mr = (MeshRenderer)gameObjectToSync.GetComponent(typeof(MeshRenderer));
				MeshCollider mc = (MeshCollider)gameObjectToSync.GetComponent(typeof(MeshCollider));
				
				if(mf.sharedMesh != null)
				{
					DestroyImmediate(mf.sharedMesh);
				}
				
		        mf.sharedMesh = mesh;
				mc.sharedMesh = mesh;
				
				mr.material = colouredCubesMaterial;
				
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
		Debug.Log("Getting position for node handle = " + nodeHandle);
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
	
	Mesh BuildMeshFromNodeHandle(uint nodeHandle)
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
		
		Mesh mesh = new Mesh();
        mesh.name = "testMesh";

        mesh.Clear();		
		
        Vector3[] vertices = new Vector3[resultVertLength / 4];
		Vector3[] normals = new Vector3[resultVertLength / 4];
		Vector4[] tangents = new Vector4[resultVertLength / 4];
		Vector2[] uv = new Vector2[resultVertLength / 4];
		for(int ct = 0; ct < resultVertLength / 4; ct++)
		{
			//vertices[ct] = new Vector3(resultVertices[ct * 4 + 0], resultVertices[ct * 4 + 1], resultVertices[ct * 4 + 2]);
			vertices[ct] = new Vector3(myVertices[ct].x, myVertices[ct].y, myVertices[ct].z);
			normals[ct] = new Vector3(0.0f, 0.0f, 1.0f); // Dummy normals required by Unity
			tangents[ct] = new Vector4(1.0f, 0.0f, 0.0f, 1.0f); // Dummy tangents required by Unity
			
			UInt32 colour = (UInt32)myVertices[ct].colour;
			UInt32 red = (UInt32)((colour >> 0) & 0xF) * 16;
			UInt32 green = (UInt32)((colour >> 4) & 0xF) * 16;
			UInt32 blue = (UInt32)((colour >> 8) & 0xF) * 16;
			//UInt32 alpha = (UInt32)((colour >> 12) & 0xF) * 16;
			
			float colourAsFloat = (float)(red * 65536 + green * 256 + blue);
			
			//float colourAsFloat = (float)green / 15.0f;
			
			uv[ct] = new Vector2(colourAsFloat, colourAsFloat);
			//uv[ct] = new Vector2(0.0f, 0.0f);
		}
		mesh.vertices = vertices; 
		mesh.normals = normals;
		mesh.tangents = tangents;
		mesh.triangles = resultIndices;
		mesh.uv = uv;
		
		return mesh;
	}
}
