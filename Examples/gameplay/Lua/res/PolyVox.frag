#extension GL_OES_standard_derivatives : enable //http://www.khronos.org/registry/gles/extensions/OES/OES_standard_derivatives.txt

#ifdef OPENGL_ES
precision highp float;
#endif

// Uniforms
uniform vec4 u_diffuseColor;                    // Diffuse color
uniform vec3 u_ambientColor;                    // Ambient color
uniform vec3 u_lightColor;                      // Light color
uniform vec3 u_lightDirection;       	        // Light direction
uniform sampler2D u_texture0;

// Inputs
varying vec4 v_worldSpacePosition;
varying vec4 v_color;

vec3 v_normalVector;

// Lighting
#include "shaders/lighting.frag"
#if defined(POINT_LIGHT)
#include "shaders/lighting-point.frag"
#elif defined(SPOT_LIGHT)
#include "shaders/lighting-spot.frag"
#else
#include "shaders/lighting-directional.frag"
#endif

void main()
{
	// Base color
    vec4 baseColor = v_color;

    // Normalize the vectors.
    vec3 lightDirection = normalize(u_lightDirection);
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
    vec3 noise = texture2D(u_texture0, noiseTextureSmaplePos).rgb; // Sample the texture.
    noise = noise * 2.0 - 1.0; // Adjust range to be -1.0 to +1.0
    noise *= noiseStrength; // Scale to desired strength.
    
    _baseColor = vec4(baseColor.rgb, 1.0);
    _ambientColor = u_ambientColor;
    _diffuseColor = u_lightColor;
    
    vec3 result = getLitPixel();

    // Light the pixel
    gl_FragColor.a = baseColor.a;
    gl_FragColor.rgb = result + noise;
}