// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"

#include "CubiquityC.h"

class SmoothVertex
{
public:
	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint16_t normal;
	uint8_t m0;
	uint8_t m1;
	uint8_t m2;
	uint8_t m3;
	uint8_t m4;
	uint8_t m5;
	uint8_t m6;
	uint8_t m7;
};

uint32_t noOfIndices;
uint32_t* indices;
uint32_t noOfVertices;
SmoothVertex* vertices;

void validate(int returnCode)
{
	if (returnCode != CU_OK)
	{
		std::cout << cuGetErrorCodeAsString(returnCode) << " : " << cuGetLastErrorMessage() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void processOctreeNode(uint32_t octreeNodeHandle)
{
	int32_t nodeX, nodeY, nodeZ;
	cuGetNodePositionMC(octreeNodeHandle, &nodeX, &nodeY, &nodeZ);

	std::cout << "Node position: " << nodeX << " " << nodeY << " " << nodeZ << std::endl;

	uint32_t hasMesh;
	validate(cuNodeHasMesh(octreeNodeHandle, &hasMesh));
	if (hasMesh == 1)
	{
		validate(cuGetNoOfIndicesMC(octreeNodeHandle, &noOfIndices));
		validate(cuGetIndicesMC(octreeNodeHandle, &indices));

		validate(cuGetNoOfVerticesMC(octreeNodeHandle, &noOfVertices));
		validate(cuGetVerticesMC(octreeNodeHandle, (float**)(&vertices)));

		std::cout << "Found mesh - it has " << noOfVertices << " vertices and " << noOfIndices << " indices." << std::endl;
	}

	for (uint32_t z = 0; z < 2; z++)
	{
		for (uint32_t y = 0; y < 2; y++)
		{
			for (uint32_t x = 0; x < 2; x++)
			{
				uint32_t hasChildNode;
				validate(cuHasChildNodeMC(octreeNodeHandle, x, y, z, &hasChildNode));

				if (hasChildNode == 1)
				{
					uint32_t childNodeHandle;
					validate(cuGetChildNodeMC(octreeNodeHandle, x, y, z, &childNodeHandle));

					// Recursivly call the octree traversal
					processOctreeNode(childNodeHandle);
				}
			}
		}
	}
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "VertexShader.glsl", "FragmentShader.glsl" );

	// Get a handle for our "MVP" uniform
	GLuint modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
	GLuint viewMatrixID = glGetUniformLocation(programID, "viewMatrix");
	GLuint projectionMatrixID = glGetUniformLocation(programID, "projectionMatrix");

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static GLushort g_vertex_buffer_data[] = { 
		0000000,0000000,0000000, 1, 2, 3, 4, 5,
		0000000, 0000000, 128*256, 1, 2, 3, 4, 5,
		0000000, 128*256, 128*256, 1, 2, 3, 4, 5,
		128*256, 128*256, 0000000, 1, 2, 3, 4, 5,
		0000000, 0000000, 0000000, 1, 2, 3, 4, 5,
		0000000, 128*256, 0000000, 1, 2, 3, 4, 5,
		128*256, 0000000, 128*256, 1, 2, 3, 4, 5,
		0000000, 0000000, 0000000, 1, 2, 3, 4, 5,
		128*256, 0000000, 0000000, 1, 2, 3, 4, 5,
		128*256, 128*256, 0000000, 1, 2, 3, 4, 5,
		128*256, 0000000, 0000000, 1, 2, 3, 4, 5,
		0000000, 0000000, 0000000, 1, 2, 3, 4, 5,
		0000000, 0000000, 0000000, 1, 2, 3, 4, 5,
		0000000, 128*256, 128*256, 1, 2, 3, 4, 5,
		0000000, 128*256, 0000000, 1, 2, 3, 4, 5,
		128*256, 0000000, 128*256, 1, 2, 3, 4, 5,
		0000000, 0000000, 128*256, 1, 2, 3, 4, 5,
		0000000, 0000000, 0000000, 1, 2, 3, 4, 5,
		0000000, 128*256, 128*256, 1, 2, 3, 4, 5,
		0000000, 0000000, 128*256, 1, 2, 3, 4, 5,
		128*256, 0000000, 128*256, 1, 2, 3, 4, 5,
		128*256, 128*256, 128*256, 1, 2, 3, 4, 5,
		128*256, 0000000, 0000000, 1, 2, 3, 4, 5,
		128*256, 128*256, 0000000, 1, 2, 3, 4, 5,
		128*256, 0000000, 0000000, 1, 2, 3, 4, 5,
		128*256, 128*256, 128*256, 1, 2, 3, 4, 5,
		128*256, 0000000, 128*256, 1, 2, 3, 4, 5,
		128*256, 128*256, 128*256, 1, 2, 3, 4, 5,
		128*256, 128*256, 0000000, 1, 2, 3, 4, 5,
		0000000, 128*256, 0000000, 1, 2, 3, 4, 5,
		128*256, 128*256, 128*256, 1, 2, 3, 4, 5,
		0000000, 128*256, 0000000, 1, 2, 3, 4, 5,
		0000000, 128*256, 128*256, 1, 2, 3, 4, 5,
		128*256, 128*256, 128*256, 1, 2, 3, 4, 5,
		0000000, 128*256, 128*256, 1, 2, 3, 4, 5,
		128*256, 0000000, 128*256, 1, 2, 3, 4, 5
	};

	uint32_t volumeHandle;
	validate(cuNewTerrainVolumeFromVDB("C:/code/cubiquity/Data/Volumes/Version 0/SmoothVoxeliensTerrain.vdb", 32, &volumeHandle));

	validate(cuUpdateVolumeMC(volumeHandle));

	uint32_t hasRootNode;
	validate(cuHasRootOctreeNodeMC(volumeHandle, &hasRootNode));
	if (hasRootNode == 1)
	{
		uint32_t octreeNodeHandle;
		cuGetRootOctreeNodeMC(volumeHandle, &octreeNodeHandle);
		processOctreeNode(octreeNodeHandle);
	}

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SmoothVertex) * noOfVertices, vertices, GL_STATIC_DRAW);

	do
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 modelMatrix = glm::mat4(1.0);
		glm::mat4 viewMatrix = getViewMatrix();
		glm::mat4 projectionMatrix = getProjectionMatrix();

		// Send our transformations to the currently bound shader
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &viewMatrix[0][0]);
		glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, &projectionMatrix[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribIPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_UNSIGNED_SHORT,           // type
			//GL_FALSE,           // normalized?
			16,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_POINTS, 0, noOfVertices); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	// Delete the volume from memory (doesn't delete from disk).
	validate(cuDeleteTerrainVolume(volumeHandle));

	return 0;
}

