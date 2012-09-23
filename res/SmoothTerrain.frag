#ifdef OPENGL_ES
precision highp float;
#endif

uniform vec3 u_ambientColor;                    // Ambient color
uniform vec3 u_lightColor;                      // Light color
uniform vec3 u_lightDirection;       	        // Light direction

varying vec3 v_normal;

void main()
{	
    // Base color
    vec4 baseColor = vec4(0.0,0.0,1.0,1.0);

    // Normalize the vectors.
    vec3 lightDirection = normalize(u_lightDirection);
    vec3 normalVector = v_normal;
    
    // Compute noise. All colour channels get the same value. Use model
    // space position, or perhaps banding occurs for larger terrains?
    //const float noiseStrength = 0.10;
    //vec3 noise = vec3(positionBasedNoise(vec4(v_modelSpacePosition.xyz, noiseStrength)));

    // Ambient
    vec3 ambientColor = baseColor.rgb * u_ambientColor;

    // Diffuse
	float attenuation = 1.0;
    float ddot = dot(normalVector, -lightDirection); //Vector to light is inverse of light direction
    float intensity =  max(0.0, attenuation * ddot);
    vec3 diffuseColor = u_lightColor * baseColor.rgb * intensity;

    // Light the pixel
    gl_FragColor.a = baseColor.a;
    gl_FragColor.rgb = ambientColor + diffuseColor/* + noise*/;
}