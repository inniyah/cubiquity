#version 330 core

// Interpolated values from the vertex shaders
//in vec2 UV;
in vec3 worldNormal;
in vec4 materialsOut;

// Ouput data
out vec3 color;

void main()
{
	// Vertex colors coming out of Cubiquity don't actually sum to one
	// (roughly 0.5 as that's where the isosurface is). Make them sum
	// to one, though Cubiquity should probably be changed to do this.
	vec4 materialWeights = materialsOut;
	float materialStrengthsSum = 
		materialWeights.x + materialWeights.y + materialWeights.z + materialWeights.w;
	materialWeights /= materialStrengthsSum;
			
	// Colors taken from https://en.wikipedia.org/wiki/List_of_colors
	vec4 material0 = vec4(0.88, 0.66, 0.37, 1.0); // Earth yellow
	vec4 material1 = vec4(0.09, 0.55, 0.09, 1.0); // Forest green
	vec4 material2 = vec4(0.50, 1.00, 0.83, 1.0); // Aquamarine
	vec4 material3 = vec4(1.00, 1.00, 1.00, 1.0); // White
	
	vec4 blendedMaterial =
		material0 * materialWeights.x +
		material1 * materialWeights.y +
		material2 * materialWeights.z +
		material3 * materialWeights.w;

	// Output color = color of the texture at the specified UV
	color = vec3(blendedMaterial.xyz);
}