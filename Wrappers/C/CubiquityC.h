// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CUBIQUITYC_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CUBIQUITYC_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CUBIQUITYC_EXPORTS
#define CUBIQUITYC_API __declspec(dllexport)
#else
#define CUBIQUITYC_API __declspec(dllimport)
#endif

// This class is exported from the CubiquityC.dll
class CUBIQUITYC_API CCubiquityC {
public:
	CCubiquityC(void);
	// TODO: add your methods here.
};

extern CUBIQUITYC_API int nCubiquityC;

CUBIQUITYC_API int fnCubiquityC(void);
