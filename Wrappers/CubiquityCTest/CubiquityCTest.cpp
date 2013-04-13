// CubiquityCTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

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
	unsigned int volumeID = newSmoothTerrainVolume(0, 0, 0, 15, 15, 15, 32, 32);
	updateVolume(volumeID);
	unsigned int rootOctreeNodeHandle = getRootOctreeNode(volumeID);
	unsigned int noOfVertices = getNoOfVertices(volumeID, rootOctreeNodeHandle);
	unsigned int noOfIndices = getNoOfIndices(volumeID, rootOctreeNodeHandle);
	float* pVertices = getVertices(volumeID, rootOctreeNodeHandle);
	unsigned int* pIndices = getIndices(volumeID, rootOctreeNodeHandle);

	for(unsigned int ct = 0; ct < noOfIndices; ct++)
	{   
		tstringstream stros;    
		stros << pIndices[ct] << " ";
		OutputDebugString(stros.str().c_str());
	}

	tstringstream stros;    
	stros << std::endl << std::endl;
	OutputDebugString(stros.str().c_str());

	unsigned int floatsPerVert = 7;
	for(unsigned int ct = 0; ct < noOfVertices * floatsPerVert; )
	{
		tstringstream stros;    
		stros << pVertices[ct] << " "; ct++;
		stros << pVertices[ct] << " "; ct++;
		stros << pVertices[ct] << " "; ct++;

		stros << pVertices[ct] << " "; ct++;
		stros << pVertices[ct] << " "; ct++;
		stros << pVertices[ct] << " "; ct++;

		stros << pVertices[ct] << " "; ct++;
		/*stros << pVertices[ct] << " "; ct++;
		stros << pVertices[ct] << " "; ct++;
		stros << pVertices[ct] << " "; ct++;*/
		stros << std::endl;
		OutputDebugString(stros.str().c_str());
	}

	return 0;
}

