#version 330 core

// We pack the encoded position and the encoded normal into a single 
// vertex attribute to save space: http://stackoverflow.com/a/21680009
layout(location = 0) in uvec4 encodedPositionAndNormal;
layout(location = 1) in uvec4 materials;

// Output data ; will be interpolated for each fragment.
//out vec2 UV;
out vec3 worldNormal;
out vec4 materialsOut;

// Values that stay constant for the whole mesh.
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	vec3 decodedPosition = encodedPositionAndNormal.xyz;
	decodedPosition.xyz = decodedPosition.xyz * (1.0 / 256.0);
	
	uint encodedX = (encodedPositionAndNormal.w >> 10u) & 0x1Fu;
	uint encodedY = (encodedPositionAndNormal.w >> 5u) & 0x1Fu;
	uint encodedZ = (encodedPositionAndNormal.w) & 0x1Fu;
	worldNormal = vec3(encodedX, encodedY, encodedZ);
	worldNormal = worldNormal / 15.5;
	worldNormal = worldNormal - vec3(1.0, 1.0, 1.0);
	
	materialsOut = materials;
	
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(decodedPosition,1);
}
