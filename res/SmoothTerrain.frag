#ifdef OPENGL_ES
precision highp float;
#endif

uniform vec3 u_ambientColor;                    // Ambient color
uniform vec3 u_lightColor;                      // Light color
uniform vec3 u_lightDirection;       	        // Light direction

varying vec4 v_normalAndMaterial;
varying vec4 v_texCoord0;

void main()
{	
    vec3 texCoord0 = v_texCoord0.xyz;
    texCoord0 = normalize(texCoord0);
    
    // Base color
    vec4 baseColor = vec4(0.3,0.3,0.3,1.0);
    baseColor += texCoord0.x * vec4(1.0,0.0,0.0,1.0);
    baseColor += texCoord0.y * vec4(0.0,1.0,0.0,1.0);
    baseColor += texCoord0.z * vec4(0.0,0.0,1.0,1.0);
    gl_FragColor = baseColor;
    
    //gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    
    /*float total = v_texCoord0.x + v_texCoord0.y + v_texCoord0.z;
    vec4 baseColor = vec4(total,total,total,1.0);
    gl_FragColor = baseColor;*/
    
    /*vec4 baseColor = vec4(1.0,0.0,1.0,1.0);
    
    float material = v_normalAndMaterial.w;
    if(material > 0.5)
    {
        baseColor = vec4(1.0,0.0,0.0,1.0);
    }
    if(material > 1.5)
    {
        baseColor = vec4(0.0,1.0,0.0,1.0);
    }
    if(material > 2.5)
    {
        baseColor = vec4(0.0,0.0,1.0,1.0);
    }
    if(material > 3.5)
    {
        baseColor = vec4(1.0,1.0,1.0,1.0);
    }*/

    // Normalize the vectors.
    /*vec3 lightDirection = normalize(u_lightDirection);
    vec3 normalVector = v_normalAndMaterial.xyz;    

    // Ambient
    vec3 ambientColor = baseColor.rgb * u_ambientColor;

    // Diffuse
	float attenuation = 1.0;
    float ddot = dot(normalVector, -lightDirection); //Vector to light is inverse of light direction
    float intensity =  max(0.0, attenuation * ddot);
    vec3 diffuseColor = u_lightColor * baseColor.rgb * intensity;

    // Light the pixel
    gl_FragColor.a = baseColor.a;
    gl_FragColor.rgb = ambientColor + diffuseColor;*/
}