// Inputs
attribute vec4 a_position;                          // Vertex Position (x, y, z, w)
attribute vec3 a_normal;                          // Vertex Position (x, y, z, w)

// Uniforms
uniform mat4 u_worldViewProjectionMatrix;           // Matrix to transform a position to clip space.

varying vec3 v_normal;

void main()
{    
    //Vertex position
    vec4 modelSpacePosition = vec4(a_position.xyz, 1.0);
    
    a_position.xyz += a_normal * 5.0;
    
    //v_colour = abs(vec4(a_normal, 1.0)); //vec4(0.0, 1.0, 0.0, 1.0);
    
    v_normal = a_normal;
        
    // Transform position to clip space.
    gl_Position = u_worldViewProjectionMatrix * modelSpacePosition;
}