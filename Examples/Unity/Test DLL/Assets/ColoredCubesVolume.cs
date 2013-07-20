using UnityEngine;
using System;
using System.Collections;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

public struct CubiquityVertex 
{
	// Disable 'Field ... is never assigned to'
	// warnings as this structure is just for interop
	#pragma warning disable 0649
	public float x;
	public float y;
	public float z;
	public uint colour;
	#pragma warning restore 0649
}

[ExecuteInEditMode]
public class ColoredCubesVolume : MonoBehaviour
{	
	// This is the relative path to where the volumes are stored on disk.
	[System.NonSerialized]
	private string pathToData = "Cubiquity/Volumes/";
	
	// The name of the dataset to load from disk. A folder with this
	// name should be found in the 'pathToData' location above.
	public string datasetName = null;
	
	// The side length of an extracted mesh for the most detailed LOD.
	// Bigger values mean fewer batches but slower surface extraction.
	// For some reason Unity won't serialize uints so it's stored as int.
	public int baseNodeSize = 0;
	
	// Determines whether collision data is generated as well as a
	// renderable mesh. This does not apply when in the Unity editor.
	public bool UseCollisionMesh = true;
	
	// The extents (dimensions in voxels) of the volume.
	public Region region = null;
	
	// If this is set then we import image slices from this path.
	[System.NonSerialized]
	internal string voldatFolder;
	
	// If set, this identifies the volume to the Cubiquity DLL. It can
	// be tested against null to find if the volume is currently valid.
	[System.NonSerialized]
	internal uint? volumeHandle = null;
	
	// This corresponds to the root OctreeNode in Cubiquity.
	[System.NonSerialized]
	private GameObject rootGameObject;
	
	internal void Initialize()
	{	
		// This function might get called multiple times. E.g the user might call it striaght after crating the volume (so
		// they can add some initial data to the volume) and it might then get called again by OnEnable(). Handle this safely.
		if(volumeHandle == null)
		{	
			// If the voldatFolder is set then we initialize the volume with the suplpied data.
			if((voldatFolder != null) && (voldatFolder != ""))
			{
				// Ask Cubiquity to create a volume from the VolDat data.
				volumeHandle = CubiquityDLL.NewColoredCubesVolumeFromVolDat(voldatFolder, pathToData + datasetName + "/", (uint)baseNodeSize);
				
				// The user didn't specify a region as this is determined by the size of
				// the VolDat data, so we have to pull this information back from Cubiquity.
				int lowerX, lowerY, lowerZ, upperX, upperY, upperZ;
				CubiquityDLL.GetEnclosingRegion(volumeHandle.Value, out lowerX, out lowerY, out lowerZ, out upperX, out upperY, out upperZ);
				region = new Region(lowerX, lowerY, lowerZ, upperX, upperY, upperZ);
				
				// Set this to null so we don't import this data again. When the volume is shutdown the
				// data is flushed to the page folder, and when the volume is reinitialised the data will
				// be loaded from the page folder as with a normal volume. So there is no need to reimport.
				voldatFolder = null;
			}
			else if(region != null)
			{
				// Create an empty region of the desired size.
				volumeHandle = CubiquityDLL.NewColoredCubesVolume(region.lowerCorner.x, region.lowerCorner.y, region.lowerCorner.z,
					region.upperCorner.x, region.upperCorner.y, region.upperCorner.z, pathToData + datasetName + "/", (uint)baseNodeSize);
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
					rootGameObject = BuildGameObjectFromNodeHandle(rootNodeHandle, null);	
				}
				syncNode(rootNodeHandle, rootGameObject);
			}
		}
	}
	
	public void Shutdown(bool saveChanges)
	{
		Debug.Log("In ColoredCubesVolume.Shutdown()");
		
		if(volumeHandle.HasValue)
		{
			CubiquityDLL.DeleteColoredCubesVolume(volumeHandle.Value);
			volumeHandle = null;
			
			// Now that we've destroyed the volume handle, and volume data will have been paged into the override folder. This
			// includes any potential changes to the volume. If the user wanted to save this then copy it to the main page folder
			if(saveChanges)
			{
				foreach(var file in Directory.GetFiles(pathToData + datasetName + "/override"))
				{
					File.Copy(file, Path.Combine(pathToData + datasetName + "/", Path.GetFileName(file)), true);
				}
			}
			
			// Delete all the data in override
			// FIXME - Should probably check for a file extension.
			System.IO.DirectoryInfo overrideDirectory = new DirectoryInfo(pathToData + datasetName + "/override");
			foreach (FileInfo file in overrideDirectory.GetFiles())
			{
				file.Delete();
			}
			
			// Game objects in our tree are created with the 'DontSave' flag set, and according to the Unity docs this means
			// we have to destroy them manually. In the case of 'Destroy' the Unity docs explicitally say that it will destroy
			// transform children as well, so I'm assuming DestroyImmediate has the same behaviour.
			DestroyImmediate(rootGameObject);
		}
	}
	
	void OnEnable()
	{
		Debug.Log ("ColoredCubesVolume.OnEnable()");
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
	
	public void OnDisable()
	{
		Debug.Log ("ColoredCubesVolume.OnDisable()");
		
		// We only save if we are in editor mode, not if we are playing.
		bool saveChanges = !Application.isPlaying;
		
		Shutdown(saveChanges);
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
	
	public bool IsSurfaceVoxel(int x, int y, int z)
	{
		if(volumeHandle.HasValue)
		{
			Color32 color = new Color32();
			
			CubiquityDLL.GetVoxel(volumeHandle.Value, x, y, z, out color.r, out color.g, out color.b, out color.a);
			if(color.a < 127) return false;
			
			CubiquityDLL.GetVoxel(volumeHandle.Value, x + 1, y, z, out color.r, out color.g, out color.b, out color.a);
			if(color.a < 127) return true;
			
			CubiquityDLL.GetVoxel(volumeHandle.Value, x - 1, y, z, out color.r, out color.g, out color.b, out color.a);
			if(color.a < 127) return true;
			
			CubiquityDLL.GetVoxel(volumeHandle.Value, x, y + 1, z, out color.r, out color.g, out color.b, out color.a);
			if(color.a < 127) return true;
			
			CubiquityDLL.GetVoxel(volumeHandle.Value, x, y - 1, z, out color.r, out color.g, out color.b, out color.a);
			if(color.a < 127) return true;
			
			CubiquityDLL.GetVoxel(volumeHandle.Value, x, y, z + 1, out color.r, out color.g, out color.b, out color.a);
			if(color.a < 127) return true;
			
			CubiquityDLL.GetVoxel(volumeHandle.Value, x, y, z - 1, out color.r, out color.g, out color.b, out color.a);
			if(color.a < 127) return true;
		}
		
		return false;
	}
	
	public void SetVoxel(int x, int y, int z, Color32 color)
	{
		if(volumeHandle.HasValue)
		{
			if(x >= region.lowerCorner.x && y >= region.lowerCorner.y && z >= region.lowerCorner.z
				&& x <= region.upperCorner.x && y <= region.upperCorner.y && z <= region.upperCorner.z) // FIX THESE VALUES!
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
				Mesh renderingMesh;
				Mesh physicsMesh;
				
				BuildMeshFromNodeHandle(nodeHandle, out renderingMesh, out physicsMesh);
		
		        MeshFilter mf = (MeshFilter)gameObjectToSync.GetComponent(typeof(MeshFilter));
		        MeshRenderer mr = (MeshRenderer)gameObjectToSync.GetComponent(typeof(MeshRenderer));
				
				if(mf.sharedMesh != null)
				{
					DestroyImmediate(mf.sharedMesh);
				}
				
		        mf.sharedMesh = renderingMesh;				
				
				mr.material = new Material(Shader.Find("ColoredCubesVolume"));
				
				if(UseCollisionMesh)
				{
					MeshCollider mc = (MeshCollider)gameObjectToSync.GetComponent(typeof(MeshCollider));
					mc.sharedMesh = physicsMesh;
				}
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
		
		if(parentGameObject)
		{
			newGameObject.transform.parent = parentGameObject.transform;
			
			Vector3 parentLowerCorner = parentGameObject.GetComponent<OctreeNodeData>().lowerCorner;
			newGameObject.transform.localPosition = octreeNodeData.lowerCorner - parentLowerCorner;
		}
		else
		{
			newGameObject.transform.localPosition = octreeNodeData.lowerCorner;
		}
		
		newGameObject.hideFlags = HideFlags.DontSave;
		
		newGameObject.layer = LayerMask.NameToLayer("Volume");
		
		return newGameObject;
	}
	
	float packPosition(Vector3 position)
	{
		position.x += 0.5f;
		position.y += 0.5f;
		position.z += 0.5f;
		
		float result = position.x * 65536.0f + position.y * 256.0f + position.z;
		
		return result;
	}
	
	float packColor(uint color)
	{
		uint red = (uint)((color >> 0) & 0xF);
		uint green = (uint)((color >> 4) & 0xF);
		uint blue = (uint)((color >> 8) & 0xF);
		
		float result = (float)(red * 256 + green * 16 + blue);
		
		return result;
	}
	
	void BuildMeshFromNodeHandle(uint nodeHandle, out Mesh renderingMesh, out Mesh physicsMesh)
	{
		// At some point I should read this: http://forum.unity3d.com/threads/5687-C-plugin-pass-arrays-from-C
		
		// Create rendering and possible collision meshes.
		renderingMesh = new Mesh();		
		physicsMesh = UseCollisionMesh ? new Mesh() : null;
		
		// Get the data from Cubiquity.
		int[] indices = CubiquityDLL.GetIndices(nodeHandle);		
		CubiquityVertex[] cubiquityVertices = CubiquityDLL.GetVertices(nodeHandle);			
		
		// Create the arrays which we'll copy the data to.
        Vector3[] renderingVertices = new Vector3[cubiquityVertices.Length];		
		Vector3[] physicsVertices = UseCollisionMesh ? new Vector3[cubiquityVertices.Length] : null;
		
		for(int ct = 0; ct < cubiquityVertices.Length; ct++)
		{
			// Get the vertex data from Cubiquity.
			Vector3 position = new Vector3(cubiquityVertices[ct].x, cubiquityVertices[ct].y, cubiquityVertices[ct].z);
			UInt32 colour = cubiquityVertices[ct].colour;
			
			// Pack it for efficient vertex buffer usage.
			float packedPosition = packPosition(position);
			float packedColor = packColor(colour);
				
			// Copy it to the arrays.
			renderingVertices[ct] = new Vector3(packedPosition, packedColor, 0.0f);			
			if(UseCollisionMesh)
			{
				physicsVertices[ct] = position;
			}
		}
		
		// Assign vertex data to the meshes.
		renderingMesh.vertices = renderingVertices; 
		renderingMesh.triangles = indices;
		
		// FIXME - Get proper bounds
		renderingMesh.bounds = new Bounds(new Vector3(0.0f, 0.0f, 0.0f), new Vector3(500.0f, 500.0f, 500.0f));
		
		if(UseCollisionMesh)
		{
			physicsMesh.vertices = physicsVertices;
			physicsMesh.triangles = indices;
		}
	}
}
