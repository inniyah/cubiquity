#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in uvec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
	vec3 decodedPosition = vertexPosition_modelspace;
	decodedPosition.xyz = decodedPosition.xyz * (1.0 / 256.0);
	
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  projectionMatrix * viewMatrix * modelMatrix * vec4(decodedPosition,1);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}
