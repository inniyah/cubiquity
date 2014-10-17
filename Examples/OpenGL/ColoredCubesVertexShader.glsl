#version 330 core

// We pack the encoded position and the encoded normal into a single 
// vertex attribute to save space: http://stackoverflow.com/a/21680009
layout(location = 0) in uvec4 encodedPositionAndNormal;
//layout(location = 1) in uvec4 materialWeightsAsUBytes;

// Values that stay constant for the whole mesh.
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec3 decodePosition(uvec3 encodedPosition)
{
	//return vec3(encodedPosition) / 256.0;
	return vec3(encodedPosition) - 0.5;
}

void main()
{
	// Extract and decode the position.
	vec3 modelSpacePosition = decodePosition(encodedPositionAndNormal.xyz);
	
	// Output position of the vertex in clip space.
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(modelSpacePosition,1);
}
