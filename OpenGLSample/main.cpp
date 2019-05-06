#include <iostream>

// GLEW    
#define GLEW_STATIC   
#include "LoadShaders.h"
#include <GL/glew.h>

// GLFW    
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <soil/SOIL.h>
#include "MeshData.h"
#include "Utils.h"

#define BUFFER_OFFSET(x)  ((const void*) (x))
#define NUM_FACES 2

// Global variables
enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vColor = 1 };

GLuint VAOs[NumVAOs];
GLuint Buffers[3];
const GLuint NumVertices = 6;
GLuint textureId;

GLuint program;

Face* faces[NUM_FACES];

GLfloat cameraPosZ = 2.5f;
GLfloat cameraPosY = 1.2f;
GLfloat cameraPosX = 0.f;
GLfloat cameraRotationAngle = 0.f;

GLfloat deltaTime = 0.0f; // 当前帧和上一帧的时间差
GLfloat lastFrame = 0.0f; // 上一帧时间


// Function prototypes    
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void init(MeshData& meshData);
void display(MeshData& meshData);
void transform();

// Window dimensions    
const GLuint WIDTH = 400, HEIGHT = 400;

// The MAIN function, from here we start the application and run the game loop    
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW    
	glfwInit();
	// Set all the required options for GLFW    
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions    
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions    
	glfwSetKeyCallback(window, key_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions    
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers    
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions  
	glViewport(0, 0, WIDTH, HEIGHT);

	MeshData meshData;

	init(meshData);

	//importAsset("F:\\3DAssets\\meshes\\cube002.obj");

	// Game loop    
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions    
		glfwPollEvents();

		// Rendering
		display(meshData);

		// Swap the screen buffers    
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.    
	glfwTerminate();
	return 0;
}

void init(MeshData& meshData)
{
	glEnable(GL_CULL_FACE);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	program = loadShaders(shaders);

	glUseProgram(program);

	Position positions[4] = {
		{ -0.9f, -0.9f, 0.f },
		{ -0.9f, 0.9f, 0.f },
		{ 0.9f, -0.9f, 0.f },
		{ 0.9f, 0.9f, 0.f }
	};

	TextureCoordination texCoords[4] = {
		{ 0.f, 1.f },
		{ 0.f, 0.f },
		{ 1.f, 1.f },
		{ 1.f, 0.f }
	};

	meshData.loadObjFile("../models/plane1x1.obj");

	/*meshData.addPositions(positions, 4);
	meshData.addTextureCoords(texCoords, 4);

	GLuint indices[6] = {
		0, 1, 2,
		1, 2, 3
	};

	meshData.setIndices(indices, 6);

	meshData.lockData();*/

	//setup texture
	glGenTextures(1, &textureId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	int imageWidth, imageHeight;
	GLenum pixelType;


	/*GLfloat localPixels[] = {
	1.f, 0.f, 0.f,	1.f, 1.f,1.f,	1.f, 0.f, 0.f,
	1.f, 1.f, 1.f,	0.f, 0.f,1.f,	1.f, 1.f, 1.f,
	0.f, 1.f, 0.f,	1.f, 1.f, 1.f,	1.f, 0.f, 0.f
	};
	imageWidth = 3, imageHeight = 3, pixelType = GL_FLOAT;*/

	GLubyte *imagePixels = SOIL_load_image("../images/dota2_001.png", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB);
	pixelType = GL_UNSIGNED_BYTE;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, pixelType, imagePixels);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 3, 3, 0, GL_RGB, GL_FLOAT, imagePixels);

	glUniform1i(glGetUniformLocation(program, "tex"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void transform() {
	glm::mat4 projection = glm::perspective(glm::radians(45.f), 1.f, 1.f, 12.f);
	int projectionLocation = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

	//for testing
	glm::mat4 cameraRotation = glm::mat4(1.0f);
	GLfloat rd = glm::radians(cameraRotationAngle);
	cameraRotation = glm::rotate(cameraRotation, rd, glm::vec3(0.f, 1.f, 0.f));

	glm::vec3 cameraPosition = glm::vec3(cameraPosX, cameraPosY, cameraPosZ);
	glm::vec4 qcCameraPosition = glm::vec4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.f);
	glm::vec4 rotatedCameraPosition =  cameraRotation * qcCameraPosition;
	glm::vec3 v3RotatedCamPosition = glm::vec3(rotatedCameraPosition.x, rotatedCameraPosition.y, rotatedCameraPosition.z);

	glm::mat4 view = glm::lookAt(v3RotatedCamPosition, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	int viewLocation = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

	//perform Matrix transformation
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.1f, 0.1f, 0));
	GLfloat radians = glm::radians(0.f);
	model = glm::rotate(model, radians, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	int modelLocation = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
}

void display(MeshData& meshData)
{
	// Clear the colorbuffer
	glClear(GL_COLOR_BUFFER_BIT);

	GLfloat currentFrame = (GLfloat)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	transform();

	meshData.drawSelf(textureId);


	/*glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[2]);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const void *)0);*/

	

	glFlush();
}

// Is called whenever a key is pressed/released via GLFW    
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << ", ";
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		cameraRotationAngle -= 10.f;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		cameraRotationAngle += 10.f;
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		cameraPosY += 0.2f;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		cameraPosY -= 0.2f;
	}
}