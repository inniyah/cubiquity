//Mine
uniform mat4 u_viewProjectionMatrix;           // Matrix to transform a position to clip space.
uniform mat4 u_worldMatrix;

varying vec4 v_worldSpacePosition;
varying vec4 v_color;

#define LIGHTING

// Attributes
attribute vec4 a_position;									// Vertex position							(x, y, z, w)
attribute vec3 a_normal;									// Vertex normal							(x, y, z)
attribute vec2 a_texCoord;									// Vertex texture coordinate				(u, v)
#if defined(SKINNING)
attribute vec4 a_blendWeights;								// Vertex blend weight, up to 4				(0, 1, 2, 3) 
attribute vec4 a_blendIndices;								// Vertex blend index int u_matrixPalette	(0, 1, 2, 3)
#endif

// Uniforms
uniform mat4 u_worldViewProjectionMatrix;					// Matrix to transform a position to clip space
uniform mat4 u_inverseTransposeWorldViewMatrix;				// Matrix to transform a normal to view space
#if defined(SPECULAR) || defined(SPOT_LIGHT) || defined(POINT_LIGHT)
uniform mat4 u_worldViewMatrix;								// Matrix to tranform a position to view space
#endif
#if defined(SKINNING)
uniform vec4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];		// Array of 4x3 matrices
#endif
#if defined(SPECULAR)
uniform vec3 u_cameraPosition;                 				// Position of the camera in view space
#endif
#if defined(TEXTURE_REPEAT)
uniform vec2 u_textureRepeat;								// Texture repeat for tiling
#endif
#if defined(TEXTURE_OFFSET)
uniform vec2 u_textureOffset;								// Texture offset
#endif
#if defined(POINT_LIGHT)
uniform vec3 u_pointLightPosition;							// Position of light
uniform float u_pointLightRangeInverse;						// Inverse of light range 
#elif defined(SPOT_LIGHT)
uniform vec3 u_spotLightPosition;							// Position of light
uniform float u_spotLightRangeInverse;						// Inverse of light range
uniform vec3 u_spotLightDirection;                          // Direction of a spot light source
#else
#endif

// Varyings
varying vec3 v_normalVector;								// Normal vector in view space
varying vec2 v_texCoord;									// Texture coordinate
#if defined(SPECULAR)
varying vec3 v_cameraDirection;								// Direction the camera is looking at in tangent space
#endif
#if defined(POINT_LIGHT)
varying vec3 v_vertexToPointLightDirection;					// Direction of point light w.r.t current vertex in tangent space
varying float v_pointLightAttenuation;						// Attenuation of point light
#include "lighting-point.vert"
#elif defined(SPOT_LIGHT)
varying vec3 v_vertexToSpotLightDirection;					// Direction of the spot light w.r.t current vertex in tangent space
varying float v_spotLightAttenuation;						// Attenuation of spot light

// Lighting
#include "lighting-spot.vert"
#else
#include "lighting-directional.vert"
#endif

// Skinning
#if defined(SKINNING)
#include "skinning.vert"
#else
#include "skinning-none.vert" 
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
    //Extract material
    float material = a_position.w;
    
    ////////////////////////////////////////////////////////////////////////////////
    // Gameplay shader code starts here
    ////////////////////////////////////////////////////////////////////////////////
    // Get the position and normal
    vec4 position = getPosition();
    //vec3 normal = getNormal();
    
    // Reset 'w' ready for gameplay shader code.
    position.w = 1.0;

    // Transform position to clip space.
    gl_Position = u_worldViewProjectionMatrix * position;

    // Transform normal to view space.
	//mat3 normalMatrix = mat3(u_inverseTransposeWorldViewMatrix[0].xyz, u_inverseTransposeWorldViewMatrix[1].xyz, u_inverseTransposeWorldViewMatrix[2].xyz);
    //v_normalVector = normalMatrix * normal;

    // Apply light.
    applyLight(position);

    // Texture transformation
    //v_texCoord = a_texCoord;
    //#if defined(TEXTURE_REPEAT)
    //v_texCoord *= u_textureRepeat;
    //#endif
    //#if defined(TEXTURE_OFFSET)
    //v_texCoord += u_textureOffset;
    //#endif
    
    ////////////////////////////////////////////////////////////////////////////////
    // Back to our own code here
    ////////////////////////////////////////////////////////////////////////////////
    
    //Vertex colour
    v_color = vec4(floatToRGB(material), 1.0);
    
    //Vertex position
    v_worldSpacePosition = u_worldMatrix * position;   
}