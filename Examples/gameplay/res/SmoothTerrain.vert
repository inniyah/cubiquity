// Inputs
attribute vec4 a_position;                          // Vertex Position (x, y, z, w)
attribute vec4 a_texCoord0;
attribute vec3 a_normal;                          // Vertex Position (x, y, z, w)

// Uniforms
uniform mat4 u_worldMatrix;           // Matrix to transform a position to world space.
uniform mat4 u_viewProjectionMatrix;  // Matrix to transform a position to clip space.

//Outputs
varying vec4 v_worldSpacePosition;
varying vec3 v_normal;
varying vec4 v_texCoord0;

void main()
{    
    //Vertex position
    vec4 modelSpacePosition = vec4(a_position.xyz, 1.0);
    
    v_normal.xyz = a_normal;
    
    v_texCoord0 = a_texCoord0;
    
    v_worldSpacePosition = u_worldMatrix * modelSpacePosition;
        
    // Transform position to clip space.
    gl_Position = u_viewProjectionMatrix * v_worldSpacePosition;
}