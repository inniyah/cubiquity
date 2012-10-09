#ifdef OPENGL_ES
precision highp float;
#endif

// Uniforms
uniform vec4 u_colour;

// Fragment program
void main()
{
    gl_FragColor = u_colour;
}
