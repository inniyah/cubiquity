#ifdef OPENGL_ES
precision highp float;
#endif

// Inputs
varying vec4 v_color;
varying vec4 v_modelSpacePosition;
varying vec4 v_worldSpacePosition;

// Uniforms
uniform sampler2D u_diffuseTexture;             // Diffuse map texture
uniform sampler2D u_depthAndCones;
uniform sampler2D u_normalmapTexture;       	// Normalmap texture
uniform vec3 u_ambientColor;                    // Ambient color
uniform vec3 u_lightColor;                      // Light color
uniform vec3 u_worldSpaceLightVector;			// Points *towards* the light source
uniform float u_specularExponent;				// Specular exponent
uniform vec3 u_worldSpaceCameraPosition;
uniform mat4 u_inverseTransposeModelToWorldMatrix;

// ray intersect depth map using binary cone space leaping
// depth value stored in alpha channel (black is at object surface)
// and cone ratio stored in blue channel
void ray_intersect_relaxedcone(
	sampler2D relaxedcone_relief_map,
	inout vec3 p,
	inout vec3 v)
{
	const int cone_steps=30;
	const int binary_steps=16;
	
	vec3 p0 = p;

	v /= v.z;
	
	float dist = length(v.xy);
	
	for( int i=0;i<cone_steps;i++ )
	{
		vec4 tex = texture2D(u_depthAndCones, p.xy);

		float height = clamp(tex.x - p.z, 0.0, 1.0);
		
		float cone_ratio = tex.y;
		
		p += v * (cone_ratio * height / (dist + cone_ratio));
	}

	v *= p.z*0.5;
	p = p0 + v;

	for( int i=0;i<binary_steps;i++ )
	{
		vec4 tex = texture2D(relaxedcone_relief_map, p.xy);
		v *= 0.5;
		if (p.z<tex.w)
			p+=v;
		else
			p-=v;
	}
}

void main()
{
    // Calculate the normal vector in model space (cubes are always axis aligned in model space).
    vec3 modelSpaceNormal = normalize(cross(dFdx(v_modelSpacePosition.xyz), dFdy(v_modelSpacePosition.xyz))); 
    // This fixes normal corruption which has been seen.
    modelSpaceNormal = floor(modelSpaceNormal + vec3(0.5, 0.5, 0.5));
    
    // Compute our normal, tangent and binormal in world space.
	mat3 inverseTransposeModelToWorldMatrix = mat3(u_inverseTransposeModelToWorldMatrix[0].xyz, u_inverseTransposeModelToWorldMatrix[1].xyz, u_inverseTransposeModelToWorldMatrix[2].xyz);
    vec3 worldSpaceNormal = normalize(inverseTransposeModelToWorldMatrix * modelSpaceNormal);
    vec3 worldSpaceTangent = worldSpaceNormal.yzx;
    vec3 worldSpaceBinormal = worldSpaceNormal.zxy;
    
    // Create a transform to convert a vector to tangent space.
    mat3 worldToTangentMatrix = mat3(worldSpaceTangent.x, worldSpaceBinormal.x, worldSpaceNormal.x, worldSpaceTangent.y, worldSpaceBinormal.y, worldSpaceNormal.y, worldSpaceTangent.z, worldSpaceBinormal.z, worldSpaceNormal.z);
    
    // Transform light direction to tangent space
    vec3 tangentSpaceLightVector = worldToTangentMatrix * u_worldSpaceLightVector;
    
    // Compute the camera direction for specular lighting
    vec3 worldSpaceCameraDirection = u_worldSpaceCameraPosition - v_worldSpacePosition.xyz;
    vec3 tangentSpaceCameraDirection = worldToTangentMatrix * worldSpaceCameraDirection;
    
    //Compute texture coordinates
    vec2 texCoords = vec2(dot(v_worldSpacePosition.xyz, worldSpaceTangent), dot(v_worldSpacePosition.xyz, worldSpaceBinormal));
    //texCoords /= 9.0;
    texCoords += 0.5;
    
    // Compute noise. Ideally we would pull a noise value from a 3D texture based on the position of the voxel,
    // but gameplay only seems to support 2D textures at the moment. Therefore we store the texture 'slices'
    // above each other to give a texture which is x pixels wide and y=x*x pixels high.
    const float noiseTextureBaseSize = 16.0; //Size of our 3D texture, actually the width of our 2D replacement.
    const float noiseStrength = 0.04;
    vec3 voxelCentre = v_worldSpacePosition.xyz - (modelSpaceNormal * 0.5); // Back along normal takes us towards center of voxel.
    voxelCentre = floor(voxelCentre + vec3(0.5)); // 'floor' is more widely supported than 'round'.
    vec2 noiseTextureSmaplePos = vec2(voxelCentre.x, voxelCentre.y + voxelCentre.z * noiseTextureBaseSize);
    noiseTextureSmaplePos = noiseTextureSmaplePos / vec2(noiseTextureBaseSize, noiseTextureBaseSize * noiseTextureBaseSize);
    vec3 noise = texture2D(u_diffuseTexture, noiseTextureSmaplePos).rgb; // Sample the texture.
    noise = noise * 2.0 - 1.0; // Adjust range to be -1.0 to +1.0
    noise *= noiseStrength; // Scale to desired strength.
    
    //Form the base color by applying noise to the colour which was passed in.
    vec4 baseColor = vec4(v_color.rgb + noise, 1.0) ;    
    
    vec3 p = vec3(texCoords,0);
	vec3 v = normalize(tangentSpaceCameraDirection);
    v.z = -v.z;
    float depth = 0.1;
    v.xy *= depth;
    
    ray_intersect_relaxedcone(u_normalmapTexture, p, v);
    
    // Fetch normals from the normal map    
    vec3 tangentSpaceNormal = texture2D(u_normalmapTexture, p.xy).rgb;
	tangentSpaceNormal.xy = 2.0 * tangentSpaceNormal.xy - 1.0;
	tangentSpaceNormal.y = tangentSpaceNormal.y;
	tangentSpaceNormal.z = sqrt(1.0 - dot(tangentSpaceNormal.xy,tangentSpaceNormal.xy));
    
    float attenuation = 1.0;
    // Ambient
    vec3 ambientColor = baseColor.rgb * u_ambientColor;

    // Diffuse
    float ddot = dot(tangentSpaceNormal, tangentSpaceLightVector);
    float diffuseIntensity = attenuation * ddot;
    diffuseIntensity = max(0.0, diffuseIntensity);
    vec3 diffuseColor = u_lightColor * baseColor.rgb * diffuseIntensity;

    // Specular
    vec3 tangentSpaceHalfVector = normalize(tangentSpaceLightVector + tangentSpaceCameraDirection);
    float specularIntensity = attenuation * max(0.0, pow(dot(tangentSpaceNormal, tangentSpaceHalfVector), u_specularExponent));
    specularIntensity = max(0.0, specularIntensity);
    vec3 specularColor = u_lightColor * baseColor.rgb * specularIntensity;
	
	gl_FragColor.rgb =  ambientColor + diffuseColor + specularColor;
    gl_FragColor.a = 1.0;
}