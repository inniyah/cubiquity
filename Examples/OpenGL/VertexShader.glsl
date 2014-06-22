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

void main()
{
	// Extract and decode the position.
	vec3 modelSpacePosition = vec3(encodedPositionAndNormal.xyz) / 256.0;
	
	// Extract and decode the normal.
	uint encodedNormal = encodedPositionAndNormal.w;
	uint encodedNormalX = (encodedNormal >> 10u) & 0x1Fu;
	uint encodedNormalY = (encodedNormal >> 5u) & 0x1Fu;
	uint encodedNormalZ = (encodedNormal) & 0x1Fu;
	vec3 modelSpaceNormal = vec3(encodedNormalX, encodedNormalY, encodedNormalZ);
	modelSpaceNormal = (modelSpaceNormal / 15.5) - vec3(1.0, 1.0, 1.0);
	worldSpaceNormal = modelSpaceNormal; // Valid if we don't scale or rotate our volume.
	
	// Pass through the material weights.
	materialWeights = materialWeightsAsUBytes;
	
	// Output position of the vertex in clip space.
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(modelSpacePosition,1);
}
