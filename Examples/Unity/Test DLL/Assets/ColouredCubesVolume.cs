using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Text;

[System.Serializable]
public struct MyVertex 
{
	public float x;
	public float y;
	public float z;
	public UInt32 colour;
}

[System.Serializable]
public class ColouredCubesVolume : MonoBehaviour
{
	public int volumeHandle = -1;
	public GameObject rootGameObject;
	
	public uint Width = 128;
	public uint Height = 128;
	public uint Depth = 128;
	
	public static int counter = 0;
	
	public Material colouredCubesMaterial;
	
	public void performAwake()
	{		
		if(rootGameObject != null)
		{
			deleteGameObject(rootGameObject);
		}
		
		colouredCubesMaterial = new Material(Shader.Find("ColouredCubesVolume"));
			
		uint currentTime;
		CubiquityDLL.cuGetCurrentTime(out currentTime);
        Debug.Log("In performAwake(): Timestamp = " + currentTime);

        if (volumeHandle == -1)
        {
			uint volHand;
            CubiquityDLL.cuNewColouredCubesVolumeFromVolDat(out volHand, new StringBuilder("C:/Code/cubiquity/Examples/SliceData/VoxeliensTerrain/"), 64, 64);
			volumeHandle = (int)volHand;
            Debug.Log("Created volume: handle = " + volumeHandle);
        }
        else
        {
            Debug.Log("Volume already exists");
        }
		//volumeHandle = CubiquityDLL.cuNewColouredCubesVolume(0, 0, 0, (int)Width - 1, (int)Height - 1, (int)Depth - 1, 64, 64);
		
	}
	
	public void deleteGameObject(GameObject gameObjectToDelete)
	{
		MeshFilter mf = (MeshFilter)gameObjectToDelete.GetComponent(typeof(MeshFilter));
		Destroy(mf.mesh);
		
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
		
		Destroy(gameObjectToDelete);
	}
	
	public void performUpdate()
	{
		//Debug.Log ("performUpdate");
		counter++;
		CubiquityDLL.cuUpdateVolume((uint)volumeHandle);
		
		uint hasRootNode;
		CubiquityDLL.cuHasRootOctreeNode(out hasRootNode, (uint)volumeHandle);
		
		if(hasRootNode == 1)
		{		
			uint rootNodeHandle;
			CubiquityDLL.cuGetRootOctreeNode(out rootNodeHandle, (uint)volumeHandle);
		
			if(rootGameObject == null)
			{
				Debug.Log ("Creating Root GameObject");
				
				rootGameObject = BuildGameObjectFromNodeHandle(rootNodeHandle, gameObject);	
				//rootGameObject = new GameObject("My GameObject");
			}
			syncNode(rootNodeHandle, rootGameObject);
		}
	}
	
	void Awake()
	{
		performAwake();
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
		Debug.Log("Deleting volume with handle = " + volumeHandle);
		CubiquityDLL.cuDeleteColouredCubesVolume((uint)volumeHandle);
	}
	
	public Color32 GetVoxel(int x, int y, int z)
	{
		Color32 color = new Color32();
		if(volumeHandle > -1)
		{
			CubiquityDLL.cuGetVoxel((uint)volumeHandle, x, y, z, out color.r, out color.g, out color.b, out color.a);
		}
		return color;
	}
	
	public void SetVoxel(int x, int y, int z, Color32 color)
	{
		if(volumeHandle > -1)
		{
			byte alpha = color.a > 127 ? (byte)255 : (byte)0; // Threshold the alpha until we support transparency.
			CubiquityDLL.cuSetVoxel((uint)volumeHandle, x, y, z, color.r, color.g, color.b, alpha);
		}
	}
	
	public void syncNode(uint nodeHandle, GameObject gameObjectToSync)
	{
		uint meshLastUpdated;
		CubiquityDLL.cuGetMeshLastUpdated(out meshLastUpdated, nodeHandle);		
		OctreeNodeData octreeNodeData = (OctreeNodeData)(gameObjectToSync.GetComponent<OctreeNodeData>());
		
		//Debug.Log ("In syncNode: meshLastSyncronised = " + octreeNodeData.meshLastSyncronised + ", meshLastUpdated = " + meshLastUpdated);
		
		if(octreeNodeData.meshLastSyncronised < meshLastUpdated)
		{
			Debug.Log("Mesh data is out of date");
			
			uint nodeHasMesh;
			CubiquityDLL.cuNodeHasMesh(out nodeHasMesh, nodeHandle);
			if(nodeHasMesh == 1)
			{				
				Mesh mesh = BuildMeshFromNodeHandle(nodeHandle);	
				Debug.Log("Built mesh - now attaching");
		
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
			
			uint currentTime;
			CubiquityDLL.cuGetCurrentTime(out currentTime);
			octreeNodeData.meshLastSyncronised = (int)(currentTime);
		}		
		
		//Now syncronise any children
		for(uint z = 0; z < 2; z++)
		{
			for(uint y = 0; y < 2; y++)
			{
				for(uint x = 0; x < 2; x++)
				{
					uint hasChildNodeHandle;
					CubiquityDLL.cuHasChildNode(out hasChildNodeHandle, nodeHandle, x, y, z);
					if(hasChildNodeHandle == 1)
					{					
					
						uint childNodeHandle;
						CubiquityDLL.cuGetChildNode(out childNodeHandle, nodeHandle, x, y, z);					
						
						GameObject childGameObject = octreeNodeData.GetChild(x,y,z);
						
						if(childGameObject == null)
						{						
							Debug.Log("Creating child");
							
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
		CubiquityDLL.cuGetNodePosition(nodeHandle, out xPos, out yPos, out zPos);
		
		StringBuilder name = new StringBuilder("(" + xPos + ", " + yPos + ", " + zPos + ")");
		
		GameObject newGameObject = new GameObject(name.ToString ());
		newGameObject.AddComponent<OctreeNodeData>();
		newGameObject.AddComponent<MeshFilter>();
		newGameObject.AddComponent<MeshRenderer>();
		newGameObject.AddComponent<MeshCollider>();
		
		//newGameObject.hideFlags = HideFlags.HideInHierarchy;
		
		OctreeNodeData octreeNodeData = newGameObject.GetComponent<OctreeNodeData>();
		octreeNodeData.lowerCorner = new Vector3(xPos, yPos, zPos);
		
		newGameObject.transform.parent = parentGameObject.transform;
		
		//Vector3 translation = new Vector3(xPos, yPos, zPos);
		
		if(parentGameObject != gameObject)
		{
			Vector3 parentLowerCorner = parentGameObject.GetComponent<OctreeNodeData>().lowerCorner;
			newGameObject.transform.localPosition = octreeNodeData.lowerCorner - parentLowerCorner;
		}
		else
		{
			newGameObject.transform.localPosition = octreeNodeData.lowerCorner;
		}
		
		//newGameObject.hideFlags = HideFlags.HideAndDontSave;
		
		return newGameObject;
	}
	
	Mesh BuildMeshFromNodeHandle(uint nodeHandle)
	{
		uint noOfVertices;
		CubiquityDLL.cuGetNoOfVertices(out noOfVertices, nodeHandle);
		//Debug.Log("No of vertices = " + noOfVertices + " at " + Time.time);
		uint noOfIndices;
		CubiquityDLL.cuGetNoOfIndices(out noOfIndices, nodeHandle);
		//Debug.Log("No of indices = " + noOfIndices + " at " + Time.time);
		
		IntPtr ptrResultVerts;
		CubiquityDLL.cuGetVertices(out ptrResultVerts, nodeHandle);
		
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
		
		IntPtr ptrResultIndices;
		CubiquityDLL.cuGetIndices(out ptrResultIndices, nodeHandle);
		
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
