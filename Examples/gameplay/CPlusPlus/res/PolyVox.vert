// Inputs
attribute vec4 a_position;                          // Vertex Position (x, y, z, w)

// Uniforms
uniform mat4 u_worldViewProjectionMatrix;           // Matrix to transform a position to clip space.
uniform mat4 u_inverseTransposeWorldViewMatrix;     // Matrix to transform a normal to view space.

// Outputs
varying vec4 v_modelSpacePosition;
varying vec4 v_colour;

vec3 floatToRGB(float inputVal)
{	
	//Store the input in each component
	vec3 inputVec = vec3(inputVal, inputVal, inputVal);
	
	//Convert each component to a value in the range 0-255
	vec3 result = floor(inputVec / vec3(65536.0, 256.0, 1.0));	
	vec3 shiftedResult = vec3(0.0, result.rg) * 256.0;	
	result -= shiftedResult;
	
	//Convert to range 0-1
	result /= 255.0;
	
	//return the result	
	return result;
}

void main()
{
    //Vertex colour
    v_colour = vec4(floatToRGB(a_position.w), 1.0);
    
    //Vertex position
    v_modelSpacePosition = vec4(a_position.xyz, 1.0);
        
    // Transform position to clip space.
    gl_Position = u_worldViewProjectionMatrix * v_modelSpacePosition;
}