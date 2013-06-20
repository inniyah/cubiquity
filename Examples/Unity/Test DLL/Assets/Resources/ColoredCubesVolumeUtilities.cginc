#ifndef COLORED_CUBES_VOLUME_UTILITIES
#define COLORED_CUBES_VOLUME_UTILITIES

float3 unpackPosition(float packedPosition)
{	
	// Store the input in each component
	float3 packedVec = float3(packedPosition, packedPosition, packedPosition);
	
	// Convert each component to a value in the range 0-255      	
	float3 result = floor(packedVec / float3(65536.0, 256.0, 1.0));	      	
	float3 shiftedResult = float3(0.0, result.rg) * 256.0;	
	result -= shiftedResult;
		
	result -= float3(0.5, 0.5, 0.5);
		
	// Return the result	
	return result;
}
      
float3 unpackColor(float inputVal)
{	
	// Store the input in each component
	float3 inputVec = float3(inputVal, inputVal, inputVal);
		
	// Convert each component to a value in the range 0-255      	
	float3 result = floor(inputVec / float3(256.0, 16.0, 1.0));	      	
	float3 shiftedResult = float3(0.0, result.rg) * 16.0;	
	result -= shiftedResult;
	
	// Convert to range 0-1
	result /= 15.0;
		
	// Return the result	
	return result;
}

#endif //COLORED_CUBES_VOLUME_UTILITIES