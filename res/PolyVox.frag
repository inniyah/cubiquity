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

void main()
{
	// Base color
    vec4 baseColor = v_colour;

    // Normalize the vectors.
    vec3 lightDirection = normalize(u_lightDirection);
    vec3 normalVector = normalize(cross(dFdy(v_worldSpacePosition.xyz), dFdx(v_worldSpacePosition.xyz)));
    
    vec3 roundedPos = floor(v_worldSpacePosition.xyz + vec3(0.501, 0.501, 0.501)) ; //'floor' is more widely supported than 'round'
    float noise = 1000000.0 / dot(roundedPos, roundedPos);
    noise = fract(noise);
    noise *= 0.1;
    noise -= 0.05;

    // Ambient
    vec3 ambientColor = baseColor.rgb * u_ambientColor;

    // Diffuse
	float attenuation = 1.0;
    float ddot = dot(normalVector, lightDirection);
    float intensity =  max(0.0, attenuation * ddot);
    vec3 diffuseColor = u_lightColor * baseColor.rgb * intensity;

    // Light the pixel
    gl_FragColor.a = baseColor.a;
    gl_FragColor.rgb = ambientColor + diffuseColor + vec3(noise, noise, noise);
}