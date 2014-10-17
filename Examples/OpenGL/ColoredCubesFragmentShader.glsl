#version 330 core

// Interpolated values from the vertex shaders
in vec3 voxelColor;

// Output data
out vec3 color;

void main()
{
	// Output color = color of the texture at the specified UV
	color = voxelColor;
}