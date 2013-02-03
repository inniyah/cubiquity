// Inputs
attribute vec4 a_position;                          // Vertex Position (x, y, z, w)

// Uniforms
uniform mat4 u_worldMatrix;           // Matrix to transform a position to clip space.
uniform mat4 u_viewProjectionMatrix;           // Matrix to transform a position to clip space.
uniform mat4 u_inverseTransposeWorldViewMatrix;     // Matrix to transform a normal to view space.

// Outputs
varying vec4 v_worldSpacePosition;
varying vec4 v_colour;

vec4 floatToRGBA(float inputVal)
{	
	//Store the input in each component
	vec4 inputVec = vec4(inputVal, inputVal, inputVal, inputVal);
	
	//Convert each component to a value in the range 0-15
	vec4 result = floor(inputVec / vec4(4096.0, 256.0, 16.0, 1.0));	
	vec4 shiftedResult = vec4(0.0, result.rgb) * 16.0;	
	result -= shiftedResult;
	
	//Convert to range 0-1
	result /= 15.0;
	
	//return the result	
	return result;
}

void main()
{
    //Vertex colour
    v_colour = floatToRGBA(a_position.w);
    
    //Vertex position
    v_worldSpacePosition = u_worldMatrix * vec4(a_position.xyz, 1.0);
        
    // Transform position to clip space.
    gl_Position = u_viewProjectionMatrix * v_worldSpacePosition;
}