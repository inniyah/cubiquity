#extension GL_OES_standard_derivatives : enable //http://www.khronos.org/registry/gles/extensions/OES/OES_standard_derivatives.txt

#ifdef OPENGL_ES
precision highp float;
#endif

// Uniforms
uniform vec4 u_diffuseColor;                    // Diffuse color
uniform vec3 u_ambientColor;                    // Ambient color
uniform vec3 u_lightColor;                      // Light color
uniform vec3 u_lightDirection;       	        // Light direction

// Inputs
varying vec4 v_modelSpacePosition;
varying vec4 v_colour;

// Computes a noise value based on a voxel position (x,y,z) and a strength value (packed into w).
// Currently suggested to operate on model space positions because maybe banding occurs for the larger
// world space positions? If we have to change this then maybe modf the positions to keep them small.
float positionBasedNoise(vec4 positionAndStrength)
{
    //'floor' is more widely supported than 'round'. Offset consists of:
    //  - A small integer to push us away from the origin (prevent divide by zero)
    //  - 0.5 to perform the rounding
    //  - A tiny offset to prevent sparkes as faces are exactly on rounding boundary.
    vec3 roundedPos = floor(positionAndStrength.xyz + vec3(7.501));
    
    //Large number is arbitrary, but smaller number lead to banding.
    float noise = 1000000.0 / dot(roundedPos, roundedPos);
    noise = fract(noise);
    
    //Scale the noise
    float halfNoiseStrength = positionAndStrength.w * 0.5;
    noise = -halfNoiseStrength + positionAndStrength.w * noise; //http://www.opengl.org/wiki/GLSL_Optimizations#Get_MAD
    
    return noise;
}

void main()
{
	// Base color
    vec4 baseColor = v_colour;

    // Normalize the vectors.
    vec3 lightDirection = normalize(u_lightDirection);
    vec3 normalVector = normalize(cross(dFdy(v_modelSpacePosition.xyz), dFdx(v_modelSpacePosition.xyz)));
    
    // Compute noise. All colour channels get the same value. Use model
    // space position, or perhaps banding occurs for larger terrains?
    const float noiseStrength = 0.15;
    vec3 noise = vec3(positionBasedNoise(vec4(v_modelSpacePosition.xyz, noiseStrength)));

    // Ambient
    vec3 ambientColor = baseColor.rgb * u_ambientColor;

    // Diffuse
	float attenuation = 1.0;
    float ddot = dot(normalVector, lightDirection);
    float intensity =  max(0.0, attenuation * ddot);
    vec3 diffuseColor = u_lightColor * baseColor.rgb * intensity;

    // Light the pixel
    gl_FragColor.a = baseColor.a;
    gl_FragColor.rgb = ambientColor + diffuseColor + noise;
}