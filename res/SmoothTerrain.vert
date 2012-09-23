// Inputs
attribute vec4 a_position;                          // Vertex Position (x, y, z, w)

// Uniforms
uniform mat4 u_worldViewProjectionMatrix;           // Matrix to transform a position to clip space.

void main()
{    
    //Vertex position
    vec4 modelSpacePosition = vec4(a_position.xyz, 1.0);
        
    // Transform position to clip space.
    gl_Position = u_worldViewProjectionMatrix * modelSpacePosition;
}