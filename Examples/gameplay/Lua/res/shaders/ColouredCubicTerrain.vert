// Inputs
attribute vec4 a_position;									// Vertex Position (x, y, z, w)

// Outputs
varying vec4 v_color;
varying vec4 v_modelSpacePosition;
varying vec4 v_worldSpacePosition;

// Uniforms
uniform mat4 u_worldMatrix;								    // Matrix to tranform a position to world space
uniform mat4 u_viewProjectionMatrix;					// Matrix to transform a position to clip space

vec3 floatToRGB(float inputVal)
{	
	// Store the input in each component
	vec3 inputVec = vec3(inputVal, inputVal, inputVal);
	
	// Convert each component to a value in the range 0-255
	vec3 result = floor(inputVec / vec3(65536.0, 256.0, 1.0));	
	vec3 shiftedResult = vec3(0.0, result.rg) * 256.0;	
	result -= shiftedResult;
	
	// Convert to range 0-1
	result /= 255.0;
	
	// Return the result	
	return result;
}

void main()
{    
    // Vertex colour from w component of position
    v_color = vec4(floatToRGB(a_position.w), 1.0);
    
    // Reset 'w' for valid model space position
    v_modelSpacePosition = vec4(a_position.xyz, 1.0);

    // Vertex position
    v_worldSpacePosition = u_worldMatrix * v_modelSpacePosition;
    
    // Transform position to clip space.
    gl_Position = u_viewProjectionMatrix * v_worldSpacePosition;
}