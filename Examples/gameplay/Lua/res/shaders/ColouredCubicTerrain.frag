#ifdef OPENGL_ES
precision highp float;
#endif

// Inputs
varying vec4 v_color;
varying vec4 v_modelSpacePosition;
varying vec4 v_worldSpacePosition;

// Uniforms
uniform sampler2D u_diffuseTexture;             // Diffuse map texture
uniform sampler2D u_normalmapTexture;       	// Normalmap texture
uniform vec3 u_ambientColor;                    // Ambient color
uniform vec3 u_lightColor;                      // Light color
uniform vec3 u_worldSpaceLightDirection;					// Light direction
uniform float u_specularExponent;				// Specular exponent
uniform vec3 u_viewSpaceCameraPosition;
uniform mat4 u_modelToViewMatrix;
uniform mat4 u_inverseTransposeModelToViewMatrix;

void main()
{
    // Calculate the normal vector in model space (as would normally be passed into the vertex shader).
    vec3 modelSpaceNormal = normalize(cross(dFdx(v_modelSpacePosition.xyz), dFdy(v_modelSpacePosition.xyz))); 
    // This fixes normal corruption which has been seen.
    modelSpaceNormal = floor(modelSpaceNormal + vec3(0.5, 0.5, 0.5));
    
    // Transform the normal, tangent and binormals to view space.
	mat3 inverseTransposeWorldViewMatrix = mat3(u_inverseTransposeModelToViewMatrix[0].xyz, u_inverseTransposeModelToViewMatrix[1].xyz, u_inverseTransposeModelToViewMatrix[2].xyz);
    vec3 viewSpaceNormal = normalize(inverseTransposeWorldViewMatrix * modelSpaceNormal);
    
    vec3 tangent = viewSpaceNormal.yzx;
    vec3 binormal = viewSpaceNormal.zxy;
    
    // Create a transform to convert a vector to tangent space.
    vec3 tangentVector  = normalize(inverseTransposeWorldViewMatrix * tangent);
    vec3 binormalVector = normalize(inverseTransposeWorldViewMatrix * binormal);
    mat3 tangentSpaceTransformMatrix = mat3(tangentVector.x, binormalVector.x, viewSpaceNormal.x, tangentVector.y, binormalVector.y, viewSpaceNormal.y, tangentVector.z, binormalVector.z, viewSpaceNormal.z);
    
    // Transform light direction to tangent space
    vec3 tangentSpaceLightDirection = tangentSpaceTransformMatrix * u_worldSpaceLightDirection;
    
    // Compute the camera direction for specular lighting
	vec4 positionWorldViewSpace = u_modelToViewMatrix * v_modelSpacePosition;
    vec3 viewSpaceCameraDirection = u_viewSpaceCameraPosition - positionWorldViewSpace.xyz;
    
    //Compute texture coordinates
    vec2 texCoords = vec2(dot(v_worldSpacePosition.xyz, modelSpaceNormal.yzx), dot(v_worldSpacePosition.xyz, modelSpaceNormal.zxy));
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
    
    //baseColor *= 0.001;
    //baseColor += 1.0;

    // Light the pixel
    gl_FragColor.a = baseColor.a;
    #if defined(TEXTURE_DISCARD_ALPHA)
    if (gl_FragColor.a < 0.5)
        discard;
    #endif
    
    // Fetch normals from the normal map
    vec3 normalVector = normalize(texture2D(u_normalmapTexture, texCoords).rgb * 2.0 - 1.0);
    vec3 lightDirection = normalize(tangentSpaceLightDirection);
    
    vec3 cameraDirection = normalize(viewSpaceCameraDirection);
    
    float attenuation = 1.0;
    // Ambient
    vec3 ambientColor = baseColor.rgb * u_ambientColor;

    // Diffuse
    float ddot = dot(normalVector, lightDirection);
    float diffuseIntensity = attenuation * ddot;
    diffuseIntensity = max(0.0, diffuseIntensity);
    vec3 diffuseColor = u_lightColor * baseColor.rgb * diffuseIntensity;

    // Specular
    vec3 halfVector = normalize(lightDirection + cameraDirection);
    float specularIntensity = attenuation * max(0.0, pow(dot(normalVector, halfVector), u_specularExponent));
    specularIntensity = max(0.0, specularIntensity);
    vec3 specularColor = u_lightColor * baseColor.rgb * specularIntensity;
	
	gl_FragColor.rgb =  ambientColor + diffuseColor + specularColor;
    gl_FragColor.a = 1.0;
}