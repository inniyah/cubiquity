// Inputs
attribute vec4 a_position;									// Vertex Position							(x, y, z, w)

// Uniforms
uniform mat4 u_worldViewProjectionMatrix;					// Matrix to transform a position to clip space.

// Vertex Program
void main()
{
    // Transform position to clip space.a
    gl_Position = u_worldViewProjectionMatrix *  a_position;
}