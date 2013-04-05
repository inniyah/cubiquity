// Inputs
varying vec3 v_color;										// Output Vertex Color 
varying vec3 v_normalVector;

// Uniforms
uniform vec3 u_ambientColor;
uniform vec3 u_lightColor;
uniform vec3 u_worldSpaceLightVector;

void main()
{
    float attenuation = 1.0;
    // Ambient
    vec3 ambientColor = v_color * u_ambientColor;
    
    vec3 normal = normalize(v_normalVector);

    // Diffuse
    float ddot = dot(normal, u_worldSpaceLightVector);
    float diffuseIntensity = attenuation * ddot;
    diffuseIntensity = max(0.0, diffuseIntensity);
    vec3 diffuseColor = u_lightColor * v_color * diffuseIntensity;
    
    gl_FragColor.a = 1.0;
    gl_FragColor.rgb = ambientColor + diffuseColor;
    
    /*gl_FragColor.rgb *= 0.001;
    gl_FragColor.rgb += vec3(ddot, ddot, ddot);*/
}