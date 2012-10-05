// Inputs
attribute vec4 a_position;                          // Vertex Position (x, y, z, w)
attribute vec3 a_normal;                          // Vertex Position (x, y, z, w)

// Uniforms
uniform mat4 u_worldViewProjectionMatrix;           // Matrix to transform a position to clip space.

varying vec4 v_normalAndMaterial;

void main()
{    
    //Vertex position
    vec4 modelSpacePosition = vec4(a_position.xyz, 1.0);
    
    v_normalAndMaterial.xyz = a_normal;
    v_normalAndMaterial.w = a_position.w;
        
    // Transform position to clip space.
    gl_Position = u_worldViewProjectionMatrix * modelSpacePosition;
}