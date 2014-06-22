#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in uvec3 vertexPosition_modelspace;
layout(location = 1) in uint normal;
layout(location = 2) in uvec4 materials;

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
	vec3 decodedPosition = vertexPosition_modelspace;
	decodedPosition.xyz = decodedPosition.xyz * (1.0 / 256.0);
	
	uint encodedX = (normal >> 10u) & 0x1Fu;
	uint encodedY = (normal >> 5u) & 0x1Fu;
	uint encodedZ = (normal) & 0x1Fu;
	worldNormal = vec3(encodedX, encodedY, encodedZ);
	worldNormal = worldNormal / 15.5;
	worldNormal = worldNormal - vec3(1.0, 1.0, 1.0);
	
	materialsOut = materials;
	
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(decodedPosition,1);
}
