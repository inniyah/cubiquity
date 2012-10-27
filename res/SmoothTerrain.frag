#ifdef OPENGL_ES
precision highp float;
#endif

uniform vec3 u_ambientColor;                    // Ambient color
uniform vec3 u_lightColor;                      // Light color
uniform vec3 u_lightDirection;       	        // Light direction
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform sampler2D u_texture2;
uniform sampler2D u_texture3;

varying vec4 v_worldSpacePosition;
varying vec3 v_normal;
varying vec4 v_texCoord0;

vec4 textureTriplanar(sampler2D texture, vec3 position, vec3 normal, float scale)
{
    float invScale = 1.0 / scale;
    
    //Squaring a unit vector makes the components add to one.
    normal = normal * normal;
    
    vec4 xy = texture2D(texture, position.xy * invScale) * abs(normal.z);
    vec4 yz = texture2D(texture, position.yz * invScale) * abs(normal.x);
    vec4 zx = texture2D(texture, position.zx * invScale) * abs(normal.y);
    return xy + yz + zx;
}

void main()
{	
    // Normalize the vectors.
    vec3 lightDirection = normalize(u_lightDirection);
    vec3 normalVector = v_normal;   
    
    vec4 texCoord0 = v_texCoord0;
    
    // Base color
    vec4 baseColor = vec4(0.0,0.0,0.0,1.0);
    baseColor += texCoord0.x * textureTriplanar(u_texture0, v_worldSpacePosition.xyz, normalVector, 10.0);
    baseColor += texCoord0.y * textureTriplanar(u_texture1, v_worldSpacePosition.xyz, normalVector, 10.0);
    baseColor += texCoord0.z * textureTriplanar(u_texture2, v_worldSpacePosition.xyz, normalVector, 10.0);
    baseColor += texCoord0.w * textureTriplanar(u_texture3, v_worldSpacePosition.xyz, normalVector, 10.0);

    // Ambient
    vec3 ambientColor = baseColor.rgb * u_ambientColor;

    // Diffuse
	float attenuation = 1.0;
    float ddot = dot(normalVector, -lightDirection); //Vector to light is inverse of light direction
    float intensity =  max(0.0, attenuation * ddot);
    vec3 diffuseColor = u_lightColor * baseColor.rgb * intensity;

    // Light the pixel
    gl_FragColor.a = baseColor.a;
    gl_FragColor.rgb = ambientColor + diffuseColor;
}