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
varying vec4 v_worldSpacePosition;
varying vec4 v_colour;

// Computes a noise value based on a voxel position (x,y,z) and a strength value (packed into w).
float positionBasedNoise(vec4 positionAndStrength)
{
    //'floor' is more widely supported than 'round'. Include tiny offset to stop sparkles.
    vec3 roundedPos = floor(positionAndStrength.xyz + vec3(0.501));
    
    //Large number is arbitrary, but smaller number lead to banding. May need to change this for larger terrains?
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
    vec3 normalVector = normalize(cross(dFdy(v_worldSpacePosition.xyz), dFdx(v_worldSpacePosition.xyz)));
    
    //Compute noise. All colour channels get the same value.
    const float noiseStrength = 0.2;
    vec3 noise = vec3(positionBasedNoise(vec4(v_worldSpacePosition.xyz, noiseStrength)));

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