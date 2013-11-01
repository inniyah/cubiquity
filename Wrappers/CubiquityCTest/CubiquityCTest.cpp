// CubiquityCTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <sstream>

#include <Windows.h>

#include "CubiquityC.h"

#ifdef UNICODE
typedef std::wostringstream tstringstream;
#else
typedef std::ostringstream tstringstream;
#endif


int _tmain(int argc, _TCHAR* argv[])
{
	//unsigned int volumeID = newColoredCubesVolume(0, 0, 0, 127, 127, 127, 256, 256);
	unsigned int volumeID;
	cuNewColoredCubesVolumeFromVolDat("C:/Code/cubiquity/Examples/SliceData/VoxeliensTerrain/", "./", 256, &volumeID);
	cuUpdateVolume(volumeID);
	unsigned int rootOctreeNodeHandle;
	cuGetRootOctreeNode(volumeID, &rootOctreeNodeHandle);
	unsigned int noOfVertices;
	cuGetNoOfVertices(rootOctreeNodeHandle, &noOfVertices);
	unsigned int noOfIndices;
	cuGetNoOfIndices(rootOctreeNodeHandle, &noOfIndices);
	float* pVertices;
	cuGetVertices(rootOctreeNodeHandle, &pVertices);
	unsigned int* pIndices;
	cuGetIndices(rootOctreeNodeHandle, &pIndices);

	tstringstream stros;    
	stros << noOfVertices << " " << noOfIndices;
	OutputDebugString(stros.str().c_str());

	std::cout << noOfVertices << " " << noOfIndices << std::endl;

	cuDeleteColoredCubesVolume(volumeID);

	/*for(unsigned int ct = 0; ct < noOfIndices; ct++)
	{   
		tstringstream stros;    
		stros << pIndices[ct] << " ";
		OutputDebugString(stros.str().c_str());
	}

	tstringstream stros;    
	stros << std::endl << std::endl;
	OutputDebugString(stros.str().c_str());*/

	/*unsigned int floatsPerVert = 4;
	for(unsigned int ct = 0; ct < noOfVertices * floatsPerVert; )
	{
		tstringstream stros;    
		stros << pVertices[ct] << " "; ct++;
		stros << pVertices[ct] << " "; ct++;
		stros << pVertices[ct] << " "; ct++;

		stros << pVertices[ct] << " "; ct++;
		stros << std::endl;
		OutputDebugString(stros.str().c_str());
	}*/

	return 0;
}

