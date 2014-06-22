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

uint32_t noOfIndices;
uint32_t* indices;
uint32_t noOfVertices;
CuTerrainVertex* vertices;

class OpenGLOctreeNode
{
public:
	OpenGLOctreeNode(OpenGLOctreeNode* parent)
	{
		noOfIndices = 0;
		indexBuffer = 0;
		vertexBuffer = 0;

		this->parent = parent;

		for (uint32_t z = 0; z < 2; z++)
		{
			for (uint32_t y = 0; y < 2; y++)
			{
				for (uint32_t x = 0; x < 2; x++)
				{
					children[x][y][z] = 0;
				}
			}
		}
	}

	GLuint noOfIndices;
	GLuint indexBuffer;
	GLuint vertexBuffer;

	OpenGLOctreeNode* parent;
	OpenGLOctreeNode* children[2][2][2];
};

void validate(int returnCode)
{
	if (returnCode != CU_OK)
	{
		std::cout << cuGetErrorCodeAsString(returnCode) << " : " << cuGetLastErrorMessage() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void processOctreeNode(uint32_t octreeNodeHandle, OpenGLOctreeNode* openGLOctreeNode)
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

					openGLOctreeNode->children[x][y][z] = new OpenGLOctreeNode(openGLOctreeNode);

					// Recursivly call the octree traversal
					processOctreeNode(childNodeHandle, openGLOctreeNode->children[x][y][z]);
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

	uint32_t volumeHandle;
	validate(cuNewTerrainVolumeFromVDB("C:/code/cubiquity/Data/Volumes/Version 0/SmoothVoxeliensTerrain.vdb", 32, &volumeHandle));

	validate(cuUpdateVolumeMC(volumeHandle));

	uint32_t hasRootNode;
	validate(cuHasRootOctreeNodeMC(volumeHandle, &hasRootNode));
	if (hasRootNode == 1)
	{
		uint32_t octreeNodeHandle;
		OpenGLOctreeNode* rootOpenGLOctreeNode = new OpenGLOctreeNode(0);
		cuGetRootOctreeNodeMC(volumeHandle, &octreeNodeHandle);
		processOctreeNode(octreeNodeHandle, rootOpenGLOctreeNode);
	}

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CuTerrainVertex)* noOfVertices, vertices, GL_STATIC_DRAW);

	GLuint indexbuffer;
	glGenBuffers(1, &indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * noOfIndices, indices, GL_STATIC_DRAW);

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
		glVertexAttribIPointer(0, 3, GL_UNSIGNED_SHORT,  sizeof(CuTerrainVertex), (GLvoid*)(offsetof(CuTerrainVertex, encodedPosX)));

		glEnableVertexAttribArray(1); // Attrib '1' is the vertex normals.
		glVertexAttribIPointer(1, 1, GL_UNSIGNED_SHORT, sizeof(CuTerrainVertex), (GLvoid*)(offsetof(CuTerrainVertex, encodedNormal)));

		// Draw the triangle !
		glDrawElements(GL_TRIANGLES, noOfIndices, GL_UNSIGNED_INT, 0);

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

