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
	//unsigned int volumeID = newColouredCubesVolume(0, 0, 0, 127, 127, 127, 256, 256);
	unsigned int volumeID = newColouredCubesVolumeFromVolDat("C:/temp/VoxeliensTerrain/", 256, 256);
	updateVolume(volumeID);
	unsigned int rootOctreeNodeHandle = getRootOctreeNode(volumeID);
	unsigned int noOfVertices = getNoOfVertices(volumeID, rootOctreeNodeHandle);
	unsigned int noOfIndices = getNoOfIndices(volumeID, rootOctreeNodeHandle);
	float* pVertices = getVertices(volumeID, rootOctreeNodeHandle);
	unsigned int* pIndices = getIndices(volumeID, rootOctreeNodeHandle);

	tstringstream stros;    
	stros << noOfVertices << " " << noOfIndices;
	OutputDebugString(stros.str().c_str());

	std::cout << noOfVertices << " " << noOfIndices << std::endl;

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

