#define LIGHTING

#ifdef OPENGL_ES
precision highp float;
#endif

// Uniforms
uniform sampler2D u_diffuseTexture;             // Diffuse map texture
uniform vec3 u_ambientColor;                    // Ambient color
uniform vec3 u_lightColor;                      // Light color
uniform vec3 u_lightDirection;					// Light direction
#if defined(SPECULAR)
uniform float u_specularExponent;				// Specular exponent
#endif
#if defined(MODULATE_COLOR)
uniform vec4 u_modulateColor;               	// Modulation color
#endif
#if defined(MODULATE_ALPHA)
uniform float u_modulateAlpha;              	// Modulation alpha
#endif

// Varyings
//varying vec3 v_normalVector;                    // Normal vector in view space
varying vec2 v_texCoord;                        // Texture coordinate
#if defined(POINT_LIGHT)
varying vec3 v_vertexToPointLightDirection;		// Light direction w.r.t current vertex in tangent space.
varying float v_pointLightAttenuation;			// Attenuation of point light.
#elif defined(SPOT_LIGHT)
varying vec3 v_spotLightDirection;				// Direction of spot light in tangent space.
varying vec3 v_vertexToSpotLightDirection;		// Direction of the spot light w.r.t current vertex in tangent space.
varying float v_spotLightAttenuation;			// Attenuation of spot light.
#else
varying vec3 v_lightDirection;					// Direction of light in tangent space.
#endif
#if defined(SPECULAR)
varying vec3 v_cameraDirection;                 // Camera direction
#endif

// Inputs
varying vec4 v_worldSpacePosition;
varying vec4 v_color;

// This one is named like a varying parameter as this is how Gameplay
// expects the normal to have been passed in, but actually it is not 
// varying and we are instead calculating the normal in the fragment shader.
vec3 v_normalVector;

// Lighting 
#include "lighting.frag"
#if defined(POINT_LIGHT)
#include "lighting-point.frag"
#elif defined(SPOT_LIGHT)
uniform float u_spotLightInnerAngleCos;			// The bright spot [0.0 - 1.0]
uniform float u_spotLightOuterAngleCos;			// The soft outer part [0.0 - 1.0]
uniform vec3 u_spotLightDirection;              // Direction of a spot light source
#include "lighting-spot.frag"
#else
#include "lighting-directional.frag"
#endif


void main()
{
    // Calculate the normal vector
    v_normalVector = normalize(cross(dFdx(v_worldSpacePosition.xyz), dFdy(v_worldSpacePosition.xyz)));
    
    // Compute noise. Ideally we would pull a noise value from a 3D texture based on the position of the voxel,
    // but gameplay only seems to support 2D textures at the moment. Therefore we store the texture 'slices'
    // above each other to give a texture which is x pixels wide and y=x*x pixels high.
    const float noiseTextureBaseSize = 16.0; //Size of our 3D texture, actually the width of our 2D replacement.
    const float noiseStrength = 0.04;
    vec3 voxelCentre = v_worldSpacePosition.xyz - (v_normalVector * 0.5); // Back along normal takes us towards center of voxel.
    voxelCentre = floor(voxelCentre + vec3(0.5)); // 'floor' is more widely supported than 'round'.
    vec2 noiseTextureSmaplePos = vec2(voxelCentre.x, voxelCentre.y + voxelCentre.z * noiseTextureBaseSize);
    noiseTextureSmaplePos = noiseTextureSmaplePos / vec2(noiseTextureBaseSize, noiseTextureBaseSize * noiseTextureBaseSize);
    vec3 noise = texture2D(u_diffuseTexture, noiseTextureSmaplePos).rgb; // Sample the texture.
    noise = noise * 2.0 - 1.0; // Adjust range to be -1.0 to +1.0
    noise *= noiseStrength; // Scale to desired strength.
    
    //Form the base color by applying noise to the colour which was passed in.
    _baseColor = vec4(v_color.rgb + noise, 1.0) ;
    
    // Perfomrm lighting
    gl_FragColor.rgb = getLitPixel();
    gl_FragColor.a = 1.0;
}