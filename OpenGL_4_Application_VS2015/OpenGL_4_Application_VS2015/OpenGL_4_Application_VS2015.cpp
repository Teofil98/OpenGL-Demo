//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC

#include <iostream>
#include <math.h>
#include "glm/glm.hpp"//core glm functionality
#include "glm/gtc/matrix_transform.hpp"//glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "GLEW/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include "Shader.hpp"
#include "Camera.hpp"
#define TINYOBJLOADER_IMPLEMENTATION

#include "Model3D.hpp"
#include "Mesh.hpp"
#include "SkyBox.hpp"

//#define SHADOW_WIDTH 1;
//#define SHADOW_HEIGHT 1;

int SHADOW_WIDTH = 2048;
int SHADOW_HEIGHT = 2048;

//int SHADOW_WIDTH = 4096;
//int SHADOW_HEIGHT = 4096;


//int glWindowWidth = 640;
//int glWindowHeight = 480;

//int glWindowWidth = 1640;
//int glWindowHeight = 1480;

int glWindowWidth = 1920;
int glWindowHeight = 1080;

int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

glm::mat4 lightSpaceTrMatrix;
GLuint lightSpaceTrMatrixLoc;



glm::vec3 cameraTarget;
gps::Camera myCamera(glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 0.0f, -120.0f));


//...................XXX.......................//
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f,0.2f,15.0f),
	glm::vec3(2.0f,0.0f,1.0f)
	

};
GLuint pointLightPositionsLoc;
GLuint pointLightPositionsLoc2;

glm::vec3 pointLightColor[] =
{
	glm::vec3(255.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f)
};
GLuint pointLightColorLoc;
GLuint pointLightColorLoc2;
//..........................................//

float deltaCamera = 0;
float cameraSpeed = 3.0f; //units per second

bool pressedKeys[1024];

float deltaAngle = 0;
float angle = 0.1f; //units per second



const GLfloat near_plane = 1.0f, far_plane = 50.0f;

gps::Shader myCustomShader;
gps::Shader depthMapShader;
gps::Shader colorShader;


//create shadowMap
GLuint shadowMapFBO;


//...................... ANIMATION GLOBALS ...............................//
float ufoInitialPos = -20;
float ufoMaxPos = 400;
float ufoMinPos = -20;


float ufoRotateSpeed = 20.0f;
float ufoRotateAngle = 0.0f;
float ufoMovementSpeed = 6.0f;
float ufoMovement = 0.0f;
bool ufoLightOn = false;
int ufoFramesPassed = 0;

float nanosuitPosX = 0.0f;
float nanosuitPosZ = 0.0f;
float nanosuitMovementSpeed = 4.0f;
float nanosuitRotateSpeed = 170.0f;
float nanosuitAngle = 0.0f;
float nanosuitMaxX = 20.0f;
float nanosuitMinX = -20.0f;
float nanosuitMaxZ = 20.0f;
float nanosuitMinZ = -20.0f;

bool animateCamera = false;
bool phase1 = false;
bool phase2 = false;
bool phase3 = false;
bool phase4 = false;
bool phase5 = false;
bool phase6 = false;



float planetRotateSpeed = 10.0f;
float planetRotateSpeed2 = 5.0f;
float planetRotateAngle = 0.0f;
float planetRotateAngle2 = 0.0f;
float planetRotateAngle3 = 0.0f;
float planetRotateAngle4 = 0.0f;

bool boardUfo = false;
int boardUfoFramesPassed;
bool firstFrameCameraAnimation = true;
int cameraFramePassed;
bool nanosuitBoarded = false;

bool boardPhase1 = false;
bool boardPhase2 = false;
bool boardPhase3 = false;
bool boardPhase4 = false;
bool drawNanosuit = true;
float color1 = 0;
float color2 = 0;
float nanosuitPosY = 0;
float beamPower = 0.2;

//........................................................................//

glm::vec3 sphereColor;
GLuint sphereColorLoc;

#define xMin  0
#define xMax  1
#define zMin  2
#define zMax  3
glm::vec4 boundingBoxes[] =
{
	glm::vec4(6.5f, 13.4f, -6.0f, 7.0f)
};


double lastTimeStamp = glfwGetTime();
double currentTimeStamp;
double elapsedTime;


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	myCustomShader.useShaderProgram();

	//set projection matrix
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	//send matrix data to shader
	GLint projLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set Viewport transform
	glViewport(0, 0, retina_width, retina_height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}


float lastX = 400, lastY = 300;
float yaw = 0, pitch = 0;
bool firstMouse;
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (!animateCamera)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.05f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 target;
		target.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		target.y = sin(glm::radians(pitch));
		target.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		cameraTarget = glm::normalize(target);
		myCamera.setCameraDirection(cameraTarget);
		view = myCamera.getViewMatrix();
		viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}
}

bool boundingBoxIntersect()
{

	if (nanosuitPosX > boundingBoxes[0][xMin] && nanosuitPosX < boundingBoxes[0][xMax] && nanosuitPosZ > boundingBoxes[0][zMin] && nanosuitPosZ < boundingBoxes[0][zMax])
		return true;
	return false;
}

bool unboardUFO = false;

void processMovement()
{
	if (pressedKeys[GLFW_KEY_P])
	{
		if (cameraFramePassed == 0)
		{
			animateCamera = !animateCamera;

			if (animateCamera)
				firstFrameCameraAnimation = true;

			cameraFramePassed = 60;
		}
	}

	if (pressedKeys[GLFW_KEY_B])
	{
		if (boardUfoFramesPassed == 0 && !nanosuitBoarded)
		{
			boardUfo = !boardUfo;
			boardUfoFramesPassed = 30;
		}
	}

	if (pressedKeys[GLFW_KEY_U])
	{
		if (!unboardUFO && nanosuitBoarded && !drawNanosuit)
		{
			unboardUFO = true;
			
		}
	}

	if (pressedKeys[GLFW_KEY_LEFT_SHIFT])
	{

		if (pressedKeys[GLFW_KEY_LEFT])
		{
			if (!nanosuitBoarded && !unboardUFO)
			{
				nanosuitAngle -= nanosuitRotateSpeed * elapsedTime;
				if (angle < 0.0f)
					angle += 360.0f;
			}
		}

		if (pressedKeys[GLFW_KEY_RIGHT])
		{
			if (!nanosuitBoarded && !unboardUFO)
			{
				nanosuitAngle += nanosuitRotateSpeed * elapsedTime;
				if (angle > 360.0f)
					angle -= 360.0f;
			}
		}

		if (pressedKeys[GLFW_KEY_R])
		{
			angle = 0;
		}
	}
	else
	{
		if (!nanosuitBoarded && !unboardUFO)
		{
			if (pressedKeys[GLFW_KEY_LEFT])
			{
				float interNanoPos = nanosuitPosX;

				nanosuitPosX -= nanosuitMovementSpeed * elapsedTime;
				if (nanosuitPosX < nanosuitMinX)
					nanosuitPosX = nanosuitMinX;

				if (boundingBoxIntersect())
					nanosuitPosX = interNanoPos;
			}

			if (pressedKeys[GLFW_KEY_RIGHT])
			{
				float interNanoPos = nanosuitPosX;

				nanosuitPosX += nanosuitMovementSpeed * elapsedTime;
				if (nanosuitPosX > nanosuitMaxX)
					nanosuitPosX = nanosuitMaxX;

				if (boundingBoxIntersect())
					nanosuitPosX = interNanoPos;

			}
		}
	}

		if (pressedKeys[GLFW_KEY_UP] && !nanosuitBoarded && !unboardUFO)
		{
			float interNanoPos = nanosuitPosZ;

			nanosuitPosZ -= nanosuitMovementSpeed * elapsedTime;
			if (nanosuitPosZ < nanosuitMinZ)
				nanosuitPosZ = nanosuitMinZ;

			if (boundingBoxIntersect())
				nanosuitPosZ = interNanoPos;
		}

		if (pressedKeys[GLFW_KEY_DOWN] && !nanosuitBoarded && !unboardUFO)
		{
			float interNanoPos = nanosuitPosZ;

			nanosuitPosZ += nanosuitMovementSpeed * elapsedTime;
			if (nanosuitPosZ > nanosuitMaxZ)
				nanosuitPosZ = nanosuitMaxZ;

			if (boundingBoxIntersect())
				nanosuitPosZ = interNanoPos;
		}

	

	if (pressedKeys[GLFW_KEY_KP_1])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (pressedKeys[GLFW_KEY_KP_2])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	if (pressedKeys[GLFW_KEY_KP_3])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}


	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 30 * elapsedTime;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	
	if (pressedKeys[GLFW_KEY_L])
	{	
		

		if (ufoFramesPassed == 0)
		{
			ufoLightOn = !ufoLightOn;
			ufoFramesPassed = 30;
		}

		if (boardUfo)
			ufoLightOn = false;

	}


	if (pressedKeys[GLFW_KEY_Q]) {
		angle += 30 * elapsedTime;
		if (angle > 360.0f)
			angle -= 360.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle -= 30  * elapsedTime;
		if (angle < 0.0f)
			angle += 360.0f;
	}

	if (!animateCamera)
	{

		if (pressedKeys[GLFW_KEY_W]) {

			myCamera.move(gps::MOVE_FORWARD, cameraSpeed * elapsedTime);
		}

		if (pressedKeys[GLFW_KEY_S]) {

			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed * elapsedTime);
		}

		if (pressedKeys[GLFW_KEY_A]) {

			myCamera.move(gps::MOVE_LEFT, cameraSpeed * elapsedTime);
		}

		if (pressedKeys[GLFW_KEY_D]) {

			myCamera.move(gps::MOVE_RIGHT, cameraSpeed * elapsedTime);
		}

		if (pressedKeys[GLFW_KEY_R]) {

			myCamera.move(gps::MOVE_UP, cameraSpeed * elapsedTime);
		}

		if (pressedKeys[GLFW_KEY_F]) {

			myCamera.move(gps::MOVE_DOWN, cameraSpeed * elapsedTime);
		}
	}

	if (pressedKeys[GLFW_KEY_1])
	{
		sphereColor = glm::vec3(255.0f, 255.0f, 255.0f);
		pointLightColor[1] = glm::vec3(0.0f, 0.0f, 0.0f);

		myCustomShader.useShaderProgram();
		glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor[1]));

		colorShader.useShaderProgram();
		glUniform3fv(sphereColorLoc, 1, glm::value_ptr(sphereColor));

	}

	if (pressedKeys[GLFW_KEY_2])
	{
		sphereColor = glm::vec3(255.0f, 0.0f, 0.0f);
		pointLightColor[1] = glm::vec3(255.0f, 0.0f, 0.0f);

		myCustomShader.useShaderProgram();
		glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor[1]));

		colorShader.useShaderProgram();
		glUniform3fv(sphereColorLoc, 1, glm::value_ptr(sphereColor));

	}

	if (pressedKeys[GLFW_KEY_3])
	{
		sphereColor = glm::vec3(0.0f, 255.0f, 0.0f);
		pointLightColor[1] = glm::vec3(0.0f, 255.0f, 0.0f);

		myCustomShader.useShaderProgram();
		glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor[1]));

		colorShader.useShaderProgram();
		glUniform3fv(sphereColorLoc, 1, glm::value_ptr(sphereColor));

	}

	if (pressedKeys[GLFW_KEY_4])
	{
		sphereColor = glm::vec3(0.0f, 0.0f, 255.0f);
		pointLightColor[1] = glm::vec3(0.0f, 0.0f, 255.0f);

		myCustomShader.useShaderProgram();
		glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor[1]));

		colorShader.useShaderProgram();
		glUniform3fv(sphereColorLoc, 1, glm::value_ptr(sphereColor));

	}

	if (pressedKeys[GLFW_KEY_5])
	{
		sphereColor = glm::vec3(255.0f, 0.0f, 255.0f);
		pointLightColor[1] = glm::vec3(255.0f, 0.0f, 255.0f);

		myCustomShader.useShaderProgram();
		glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor[1]));

		colorShader.useShaderProgram();
		glUniform3fv(sphereColorLoc, 1, glm::value_ptr(sphereColor));

	}

	if (pressedKeys[GLFW_KEY_6])
	{
		sphereColor = glm::vec3(0.0f, 255.0f, 255.0f);
		pointLightColor[1] = glm::vec3(0.0f, 255.0f, 255.0f);

		myCustomShader.useShaderProgram();
		glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor[1]));

		colorShader.useShaderProgram();
		glUniform3fv(sphereColorLoc, 1, glm::value_ptr(sphereColor));

	}

	if (pressedKeys[GLFW_KEY_7])
	{
		sphereColor = glm::vec3(255.0f, 255.0f, 0.0f);
		pointLightColor[1] = glm::vec3(255.0f, 255.0f, 0.0f);

		myCustomShader.useShaderProgram();
		glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor[1]));

		colorShader.useShaderProgram();
		glUniform3fv(sphereColorLoc, 1, glm::value_ptr(sphereColor));

	}

}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	//for Mac OS X
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwMakeContextCurrent(glWindow);

	glfwWindowHint(GLFW_SAMPLES, 4);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}
GLuint depthMapTexture;
GLuint textureSkyBoxID;
void initOpenGLState()
{
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	//generate FBO id
	glGenFramebuffers(1, &shadowMapFBO);


	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 5, 5, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture,0);

	//bind nothinf to color and stencil
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	//unbind frame buffer until ready to use it
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	
	//glGenTextures(1, &textureSkyBoxID);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, textureSkyBoxID);


}


gps::Model3D nanosuit;
gps::Model3D grass;
gps::Model3D ufo;
gps::Model3D cottage;
gps::Model3D tree1;
gps::Model3D starDestroyer;
gps::Model3D endor;
gps::Model3D geonosis;
gps::Model3D hoth;
gps::Model3D moon;
gps::Model3D windmill;
gps::Model3D bench;
gps::Model3D dog;
gps::Model3D deer;
gps::Model3D hare;
gps::Model3D mallard;
gps::Model3D snek;
gps::Model3D lamp;
gps::Model3D sphere;

std::vector<const GLchar*> skyBoxFaces;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
void initModels()
{
	nanosuit = gps::Model3D("objects/nanosuit/nanosuit.obj", "objects/nanosuit/");
	grass = gps::Model3D("objects/Grass2/10450_Rectangular_Grass_Patch_v1_iterations-2.obj", "");
	ufo = gps::Model3D("objects/ufo/Low_poly_UFO.obj", "");
	cottage = gps::Model3D("objects/cottage/cottage_obj.obj", "");
	tree1 = gps::Model3D("objects/Trees/Tree3/Spruce.obj", "objects/Trees/Tree3/");
	starDestroyer = gps::Model3D("objects/stardestroyer/starwarsstardestroyer.obj", "objects/stardestroyer/");
	endor = gps::Model3D("objects/Planets/Endor/endor.obj", "objects/Planets/Endor/");
	geonosis = gps::Model3D("objects/Planets/Geonosis/geonosis.obj", "objects/Planets/Geonosis/");
	hoth = gps::Model3D("objects/Planets/Hoth/hoth.obj", "objects/Planets/Hoth/");
	moon = gps::Model3D("objects/Planets/Moon/moon.obj", "objects/Planets/Moon/");
	windmill = gps::Model3D("objects/Windmill/windmill.obj", "objects/Windmill/");
	bench = gps::Model3D("objects/WoodBench/Cgtuts_Wood_Bench_OBJ.obj", "objects/WoodBench/");
	dog = gps::Model3D("objects/Dog/12228_Dog_v1_L2.obj", "objects/Dog/");
	deer = gps::Model3D("objects/Deer/12961_White-Tailed_Deer_v1_l2.obj", "objects/Deer/");
	hare = gps::Model3D("objects/Hare/12956_WhiteHare_v1.obj", "objects/Hare/");
	mallard = gps::Model3D("objects/Mallard/12248_Bird_v1_L2.obj", "objects/Mallard/");
	snek = gps::Model3D("objects/Snake/10050_RattleSnake_v4_L3.obj", "objects/Snake/");
	lamp = gps::Model3D("objects/Lamp/11832_Lamp_v1_l2.obj", "objects/Lamp/");
	sphere = gps::Model3D("objects/Sphere/sphere.obj", "objects/Sphere/");
	



	skyBoxFaces.push_back("textures/skybox/hills_ft.tga");
	skyBoxFaces.push_back("textures/skybox/hills_bk.tga");
	skyBoxFaces.push_back("textures/skybox/hills_up.tga");
	skyBoxFaces.push_back("textures/skybox/hills_dn.tga");
	skyBoxFaces.push_back("textures/skybox/hills_rt.tga");
	skyBoxFaces.push_back("textures/skybox/hills_lf.tga");


	mySkyBox.Load(skyBoxFaces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();

	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));

	

}

void initShaders()
{
	

	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	depthMapShader.loadShader("shaders/shaderLight.vert", "shaders/shaderLight.frag");
	colorShader.loadShader("shaders/colorShader.vert", "shaders/colorShader.frag");
	myCustomShader.useShaderProgram();
}

void initUniforms()
{

	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//....................XXX...............................//
	//set point light location
	pointLightPositionsLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].position");
	//pointLightPositions[0] = view * glm::vec4( pointLightPositions[0],1.0f);
	glUniform3fv(pointLightPositionsLoc, 1, glm::value_ptr(pointLightPositions[0]));

	//set point light colour
	pointLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[0].color");
	glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));

	pointLightPositionsLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].position");
	//pointLightPositions[0] = view * glm::vec4( pointLightPositions[0],1.0f);
	glUniform3fv(pointLightPositionsLoc2, 1, glm::value_ptr(pointLightPositions[1]));

	//set point light colour
	pointLightColorLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "pointLights[1].color");
	glUniform3fv(pointLightColorLoc2, 1, glm::value_ptr(pointLightColor[1]));


	colorShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(colorShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(colorShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(colorShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	sphereColor = glm::vec3(255.0f, 255.0f, 255.0f);
	sphereColorLoc = glGetUniformLocation(colorShader.shaderProgram, "color");
	glUniform3fv(sphereColorLoc, 1, glm::value_ptr(sphereColor));
	
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	if ((in_max - in_min) + out_min != 0)
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	return 0.0f;
}

void ufoAnimation()
{
	if (!boardUfo && !unboardUFO)
	{
		ufoMovement += ufoMovementSpeed * elapsedTime;
	
	}

	model = glm::translate(model, glm::vec3(0, 0, ufoMovement));

	//std::cout << map(ufoMovement, 4, 400.02, -20, 20) << " " << nanosuitPosZ << std::endl;
	
	if (ufoMovement < ufoMinPos)
		ufoMovementSpeed = -ufoMovementSpeed;

	if (ufoMovement > ufoMaxPos)
		ufoMovementSpeed = -ufoMovementSpeed;
	

	ufoRotateAngle += ufoRotateSpeed * elapsedTime;
	model = glm::rotate(model, glm::radians(ufoRotateAngle), glm::vec3(0, 1, 0));


}

void planetAnimation()
{
	
	planetRotateAngle += planetRotateSpeed * elapsedTime;

	if (planetRotateAngle > 360.0f)
		planetRotateAngle -= 360.0f;
	
	planetRotateAngle2 -= planetRotateSpeed * elapsedTime;

	if (planetRotateAngle2 < 0.0f)
		planetRotateAngle2 += 360.0f;

	planetRotateAngle3 += planetRotateSpeed2 * elapsedTime;

	if (planetRotateAngle3 > 360.0f)
		planetRotateAngle3 -= 360.0f;

	planetRotateAngle4 -= planetRotateSpeed2 * elapsedTime;

	if(planetRotateAngle4 < 0.0f)
		planetRotateAngle4 += 360.0f;
}

glm::vec3 cameraPositionInit;
glm::vec3 cameraTargetInit;

glm::vec3 cameraAnimDirection = glm::vec3(-0.0738119f, -0.308187f, -0.948458f);

void cameraAnimation()
{
	

	if (firstFrameCameraAnimation)
	{
		cameraPositionInit = myCamera.getCameraPosition();
		cameraTargetInit = myCamera.getCameraTarget();
		myCamera.setCameraPosition(glm::vec3(20.0f, 6.0f, 30.0f));
		myCamera.setCameraDirection(cameraAnimDirection);
		firstFrameCameraAnimation = false;
		phase1 = true;
	}

	if (phase1)
	{
		if(myCamera.getCameraPosition().x > -20.0f)
			myCamera.move(gps::MOVE_LEFT, 0.5 * cameraSpeed * elapsedTime);
		else
		{
			phase1 = false;
			phase2 = true;
		}
		
	}

	if (phase2)
	{
		if (myCamera.getCameraPosition().z > -20.0f)
		{
			myCamera.setCameraDirection(glm::vec3(0.897202f, -0.434446f, 0.0792841f));
			myCamera.move(gps::MOVE_LEFT, 0.5 * cameraSpeed * elapsedTime);
		}
		else
		{
			phase2 = false;
			phase3 = true;
		}
	}

	if (phase3)
	{
		if (myCamera.getCameraPosition().x < 20.0f)
		{
			myCamera.setCameraDirection(glm::vec3(-0.158873f, -0.423409f, 0.891899f));
			myCamera.move(gps::MOVE_LEFT, 0.5 * cameraSpeed * elapsedTime);
		}
		else
		{
			phase3 = false;
			phase4 = true;
		}

		
	}

	if (phase4)
	{
		if (myCamera.getCameraPosition().z < 20.0f)
		{
			myCamera.setCameraDirection(glm::vec3(-0.914257f, -0.402746f, -0.0439165));
			myCamera.move(gps::MOVE_LEFT, 0.5 * cameraSpeed * elapsedTime);
		}
		else
		{
			phase4 = false;
			myCamera.setCameraDirection(cameraPositionInit);
			myCamera.setCameraPosition(cameraPositionInit);
			firstFrameCameraAnimation = true;
			animateCamera = false;
		}
	}
	
	//std::cout << myCamera.getCameraDirection().x << " " << myCamera.getCameraDirection().y << " " << myCamera.getCameraDirection().z << std::endl;

}


void drawForest(gps::Shader shader)
{
	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-7, -3, 0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::translate(model, glm::vec3(-6.0f, 0, -1.5f));
	model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0, 0, 1));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);


	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-10, -2, 11));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(18, -1, 9));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);
	/////////////////////////////////////////////////////
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(15, -1.5, 13));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(15, -1, 6));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);



	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-9, -1.7, -8));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);


	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(15, -1, 6));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	////////////////////////////////////////////////////////////

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(15, -1, -15));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(10, -1.7, -13));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(13, -1, -17));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(5, -1.8, -16));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-4, -2.7, -15));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-13, -1.3, -14));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-4, -2.7, -15));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-7, -2.4, -13));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-4, -2.7, -10));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-6, -1, -7));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	////////////////////////////////////////////////////////////////////

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-10, -1, -5));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-11, -1.4, -2));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-15, -1, 3));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-13, -1, 6));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-14, -1, 10));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-15, -1, 13));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-11, -1, 11));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-16, -1, 15));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-13, -1, 19));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(17, -1, 19));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(13, -1, 22));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	////////////////////////////////////////////////////////////////////

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-17, -1, -5));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-18, -1.4, -2));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-21, -1, 3));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-22, -1, 6));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-23, -1, 10));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-24, -1, 13));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-18, -1, 11));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-23, -1, 15));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-20, -1, 19));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(16, -1.7, 18));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(18, -1, 23));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.5));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	tree1.Draw(shader);

}

void drawPlanets(gps::Shader shader)
{
	planetAnimation();

	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-40, 30, -20));
	model = glm::rotate(model, glm::radians(planetRotateAngle), glm::vec3(1, 1, 0));
	model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.03));


	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	endor.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(40, 35, -20));
	model = glm::rotate(model, glm::radians(planetRotateAngle2), glm::vec3(1, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.03));


	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	geonosis.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, 30, 40));
	model = glm::rotate(model, glm::radians(planetRotateAngle3), glm::vec3(0, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.05));


	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	hoth.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, 2 * 26, 2 * 30));
	model = glm::rotate(model, glm::radians(planetRotateAngle2), glm::vec3(0, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, -26, -30));
	model = glm::rotate(model, glm::radians(planetRotateAngle2), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.01));


	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	moon.Draw(shader);
}


void turnUFOLightON()
{
	pointLightPositions[0] = glm::vec3(0.0f, 1.0f, map(ufoMovement, -20.0f, 400.0f, -15.0f, 20.0f) - 2.0f);


	//std::cout << pointLightPositions[0].x << " " << pointLightPositions[0].y << " " << pointLightPositions[0].z << " " << ufoMovement << std::endl;


	myCustomShader.useShaderProgram();
	glUniform3fv(pointLightPositionsLoc, 1, glm::value_ptr(pointLightPositions[0]));
}

void turnUFOLightOFF()
{
	pointLightPositions[0] = glm::vec3(0.0, -100.0f, 0.0f);
	myCustomShader.useShaderProgram();
	glUniform3fv(pointLightPositionsLoc, 1, glm::value_ptr(pointLightPositions[0]));
}


void boardUFOAnim()
{
	myCustomShader.useShaderProgram();

	if (boardPhase1)
	{
		nanosuitBoarded = true;

		color1 = 0;
		color2 = 0;

		pointLightColor[0] = glm::vec3(color1, 0.0f, color2);
		glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));

		turnUFOLightON();
		boardPhase1 = false;
		boardPhase2 = true;
	}

	if (boardPhase2)
	{
		color1++;
		color2++;

		if (color1 > 255.0f)
		{
			boardPhase2 = false;
			boardPhase3 = true;
		}
		else
		{
			pointLightColor[0] = glm::vec3(color1, 0.0f, color2);
			glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));
		}
	}

	if (boardPhase3)
	{
		nanosuitPosY += beamPower * elapsedTime;
		model = glm::translate(model, glm::vec3(0, nanosuitPosY, 0));

		//std::cout << nanosuitPosY << std::endl;

		if (nanosuitPosY > 3.31)
		{
			boardPhase3 = false;
			boardPhase4 = true;
			drawNanosuit = false;

			
		}
	}

	if (boardPhase4)
	{
		if (color1 < 0)
		{

			boardUfo = false;
			boardPhase4 = false;

			turnUFOLightOFF();

			pointLightColor[0] = glm::vec3(255.0f, 0.0f, 0.0);
			glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));
		}
		else
		{
			color1--;
			color2--;
			pointLightColor[0] = glm::vec3(color1, 0.0f, color2);
			glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));
		}

	}

	
}

bool unboardPhase1 = false;
bool unboardPhase2 = false;
bool unboardPhase3 = false;
bool unboardPhase4 = false;
bool initDone = false;

void unboardUFOAnim()
{
	myCustomShader.useShaderProgram();

	if (unboardPhase1)
	{
		//nanosuitBoarded = true;



		color1 = 0;
		color2 = 0;

		pointLightColor[0] = glm::vec3(0.0, color1, color2);
		glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));

		turnUFOLightON();
		unboardPhase1 = false;
		unboardPhase2 = true;
		initDone = true;
	}

	if (unboardPhase2)
	{
		color1++;
		color2++;

		if (color1 > 255.0f)
		{
			unboardPhase2 = false;
			unboardPhase3 = true;
			
			nanosuitPosX = 0;
			nanosuitPosZ = map(ufoMovement, 4, 400.02, -20, 20);
		}
		else
		{
			pointLightColor[0] = glm::vec3(0.0f, color1, color2);
			glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));
		}
	}

	if (unboardPhase3)
	{
		nanosuitPosY -= beamPower * elapsedTime;
		model = glm::translate(glm::mat4(0.1), glm::vec3(nanosuitPosX, nanosuitPosY, nanosuitPosZ));

		drawNanosuit = true;

		//std::cout << nanosuitPosY << std::endl;

		if (nanosuitPosY < 0)
		{
			unboardPhase3 = false;
			unboardPhase4 = true;
			

		}
	}

	if (unboardPhase4)
	{
		if (color1 < 0)
		{

			unboardUFO = false;
			unboardPhase4 = false;
			initDone = false;
			nanosuitBoarded = false;

			turnUFOLightOFF();

			pointLightColor[0] = glm::vec3(255.0f, 0.0f, 0.0);
			glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));
		}
		else
		{
			color1--;
			color2--;
			pointLightColor[0] = glm::vec3(0.0f, color1, color2);
			glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor[0]));
		}

	}


}


void renderTheScene(gps::Shader shader)
{
	//std::cout << nanosuitPosZ << std::endl;

	if (animateCamera)
		cameraAnimation();

	

	shader.useShaderProgram();

	

	modelLoc = glGetUniformLocation(shader.shaderProgram, "model");


	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(nanosuitPosX, 0, nanosuitPosZ));
	model = glm::rotate(model, glm::radians(nanosuitAngle), glm::vec3(0, 1, 0));

	if (unboardUFO)
		nanosuitAngle = 0.0f;

	if (boardUfo)
	{
		//if (abs(map(ufoMovement, 4, 400.02, -20, 20) - nanosuitPosZ) <= 1.5 && abs(nanosuitPosX) < 0.5)
		//	turnUFOLightON();
		//else turnUFOLightOFF();

		if (!nanosuitBoarded && abs(map(ufoMovement, 4, 400.02, -20, 20) - nanosuitPosZ) <= 0.3 && abs(nanosuitPosX) < 0.3)
			boardPhase1 = true;

		boardUFOAnim();
	}

	if (unboardUFO)
	{
		if (!initDone)
			unboardPhase1 = true;
		unboardUFOAnim();
	}
	shader.useShaderProgram();

	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	if(drawNanosuit)
		nanosuit.Draw(shader);


	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0, -2.3, 0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(0.17));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	grass.Draw(shader);
	
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, 0, ufoInitialPos));
	model = glm::scale(model, glm::vec3(0.1));
	
	
	ufoAnimation();

	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	ufo.Draw(shader);
	
	//model = glm::translate(model, glm::vec3(0, 11, 0));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	////////////////////////////////////////////////


	if (!boardUfo && !unboardUFO)
	{
		if (ufoLightOn)
		{
			turnUFOLightON();
		}
		else
		{
			turnUFOLightOFF();
		}
	}
	

	if (ufoFramesPassed != 0)
		ufoFramesPassed--;


	shader.useShaderProgram();
	modelLoc = glGetUniformLocation(shader.shaderProgram, "model");

	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(10, -0.8, 0));
	model = glm::rotate(model, glm::radians(85.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.35));
	


	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	cottage.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	//create model matrix
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, 0, -8));
	//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.1));



	//send model matrix data to shader	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//create normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	starDestroyer.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-10, -1, 5));
	model = glm::rotate(model, glm::radians(60.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.007));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	windmill.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(15, -0.9, 10));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
	model = glm::scale(model, glm::vec3(0.35));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	bench.Draw(shader);


	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(1, -1.13, 7));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0,0));
	model = glm::scale(model, glm::vec3(0.02));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	dog.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-9, -0.6, 9));
	model = glm::rotate(model, glm::radians(-40.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.035));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	deer.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, -1.05, 7));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.04));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	hare.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(0, -0.97, -3));
	model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.01));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	mallard.Draw(shader);

	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(-9, -0.87, 1.9));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(-9.0f), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.01));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	snek.Draw(shader);


	//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(3, -1.2,6));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.02));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	lamp.Draw(shader);

	/*//initialize the model matrix
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	//model = glm::translate(model, glm::vec3(3, -1.2, 6));
	//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	//model = glm::scale(model, glm::vec3(20));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	sphere.Draw(shader);
	*/
	drawPlanets(shader);

	drawForest(shader);


}


glm::mat4 computeLightSpaceTrMatrix()
{
	glm::mat4 lightView = glm::lookAt(15.0f * lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	//glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
}

void renderSceneDepthMap()
{
	
	

	//render the scene to the depth buffer
	depthMapShader.useShaderProgram();


	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderTheScene(depthMapShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



}

void renderSceneMainBuffer()
{
	myCustomShader.useShaderProgram();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, retina_width, retina_height);
	glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),1,GL_FALSE,	glm::value_ptr(computeLightSpaceTrMatrix()));
	//processMovement();

	//initialize the view matrix
	view = myCamera.getViewMatrix();

	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");

	//send view matrix data to shader	
   
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE5);
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 5);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	

	renderTheScene(myCustomShader);

}

void renderColorScene()
{
	colorShader.useShaderProgram();


	//initialize the view matrix
	view = myCamera.getViewMatrix();

	viewLoc = glGetUniformLocation(colorShader.shaderProgram, "view");

	//send view matrix data to shader	

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE5);
	glUniform1i(glGetUniformLocation(colorShader.shaderProgram, "shadowMap"), 5);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);

	modelLoc = glGetUniformLocation(colorShader.shaderProgram, "model");

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 1, 0));
	model = glm::translate(model, glm::vec3(3.44, 2, 5.9));
	//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(0.1));


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	//send normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


	sphere.Draw(colorShader);
}

void renderScene()
{
	currentTimeStamp = glfwGetTime();
	elapsedTime = currentTimeStamp - lastTimeStamp;
	lastTimeStamp = currentTimeStamp;

	processMovement();

	if (cameraFramePassed != 0)
		cameraFramePassed--;

	if (boardUfoFramesPassed != 0)
		boardUfoFramesPassed--;

	renderSceneDepthMap();

	
	renderSceneMainBuffer();

	 renderColorScene();


	mySkyBox.Draw(skyboxShader, view, projection);
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initModels();
	initShaders();
	initUniforms();	
	

	while (!glfwWindowShouldClose(glWindow)) {
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	//close GL context and any other GLFW resources
	glfwTerminate();

	return 0;
}
