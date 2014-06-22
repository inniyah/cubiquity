#version 330 core

// We pack the encoded position and the encoded normal into a single 
// vertex attribute to save space: http://stackoverflow.com/a/21680009
layout(location = 0) in uvec4 encodedPositionAndNormal;
layout(location = 1) in uvec4 materialWeightsAsUBytes;

// Output data
out vec3 worldSpaceNormal;
out vec4 materialWeights;

// Values that stay constant for the whole mesh.
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec3 decodePosition(uvec3 encodedPosition)
{
	return vec3(encodedPosition) / 256.0;
}

vec3 decodeNormal(uint encodedNormal)
{
	uvec3 decodedNormal = uvec3(encodedNormal);
	decodedNormal = (decodedNormal >> uvec3(10u, 5u, 0u)) & uvec3(0x1Fu, 0x1Fu, 0x1Fu);
	return (vec3(decodedNormal) / 15.5) - vec3(1.0, 1.0, 1.0);
}

void main()
{
	// Extract and decode the position.
	vec3 modelSpacePosition = decodePosition(encodedPositionAndNormal.xyz);
	
	// Extract and decode the normal.
	vec3 modelSpaceNormal = decodeNormal(encodedPositionAndNormal.w);
	worldSpaceNormal = modelSpaceNormal; // Valid if we don't scale or rotate our volume.
	
	// Pass through the material weights.
	materialWeights = materialWeightsAsUBytes;
	
	// Output position of the vertex in clip space.
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(modelSpacePosition,1);
}
