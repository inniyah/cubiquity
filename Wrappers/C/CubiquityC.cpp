// CubiquityC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CubiquityC.h"


// This is an example of an exported variable
CUBIQUITYC_API int nCubiquityC=0;

// This is an example of an exported function.
CUBIQUITYC_API int fnCubiquityC(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see CubiquityC.h for the class definition
CCubiquityC::CCubiquityC()
{
	return;
}
