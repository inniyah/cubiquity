// Inputs
attribute vec4 a_position;                          // Vertex Position (x, y, z, w)

// Uniforms
uniform mat4 u_viewProjectionMatrix;           // Matrix to transform a position to clip space.
uniform mat4 u_inverseTransposeWorldViewMatrix;     // Matrix to transform a normal to view space.

#if defined(SPECULAR)
uniform vec3 u_cameraPosition;                 				// Position of the camera in view space
#endif

// Outputs
varying vec4 v_worldSpacePosition;
varying vec4 v_color;

#if defined(SPECULAR) || defined(SPOT_LIGHT) || defined(POINT_LIGHT)
uniform mat4 u_worldViewMatrix;								// Matrix to tranform a position to view space
uniform mat4 u_worldMatrix;								    // Matrix to tranform a position to world space
#endif

#if defined(SPECULAR)
varying vec3 v_cameraDirection;								// Direction the camera is looking at in tangent space
#endif

// Lighting
#if defined(POINT_LIGHT)
varying vec3 v_vertexToPointLightDirection;					// Direction of point light w.r.t current vertex in tangent space
varying float v_pointLightAttenuation;						// Attenuation of point light
#include "shaders/lighting-point.vert"
#elif defined(SPOT_LIGHT)
varying vec3 v_vertexToSpotLightDirection;					// Direction of the spot light w.r.t current vertex in tangent space
varying float v_spotLightAttenuation;						// Attenuation of spot light
varying vec3 v_spotLightDirection;							// Direction of spot light in tangent space
#include "shaders/lighting-spot.vert"
#else
varying vec3 v_lightDirection;								// Direction of light
#include "shaders/lighting-directional.vert"
#endif

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
    v_color = vec4(floatToRGB(a_position.w), 1.0);
    
    //Vertex position
    v_worldSpacePosition = u_worldMatrix * vec4(a_position.xyz, 1.0);
    
    // Apply light.
    applyLight(a_position);
        
    // Transform position to clip space.
    gl_Position = u_viewProjectionMatrix * v_worldSpacePosition;
}