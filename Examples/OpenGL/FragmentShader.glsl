#version 330 core

// Interpolated values from the vertex shaders
//in vec2 UV;
in vec3 worldNormal;

// Ouput data
out vec3 color;

void main()
{
	// Output color = color of the texture at the specified UV
	color = vec3(worldNormal);
}